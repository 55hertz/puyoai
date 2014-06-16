#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <stdio.h>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "core/algorithm/plan.h"
#include "core/algorithm/puyo_possibility.h"
#include "core/field/core_field.h"
#include "core/puyo_color.h"

#include "evaluator.h"
#include "evaluation_feature.h"
#include "gazer.h"
#include "learning_evaluation_feature.h"

using namespace std;

const double LEARNING_COEF = 1 / 1000.0;

void updateFeature(EvaluationParams& params,
                   const EvaluationFeature& currentFeature,
                   const EvaluationFeature& teacherFeature)
{
    for (int i = 0; i < SIZE_OF_PLAN_FEATURE_PARAM; ++i) {
        EvaluationFeatureKey key = toEvaluationFeature(i);
        double dT = (currentFeature.getValue(key) - teacherFeature.getValue(key)) * LEARNING_COEF;
        cout << "learning: " << paramName << ' '
             << params.get(paramName) << ' '
             << currentFeature.getValue(paramName) << ' '
             << teacherFeature.getValue(paramName) << ' '
             << dT << endl;
        params.add(paramName, -dT);
    }

    for (int i = 0; i < SIZE_OF_PLAN_RANGE_FEATURE_PARAM; ++i) {
        SparseEvaluationFeatureKey key = toSparseEvaluationFeatureKey(i);
        double dT = (currentFeature.get(paramName) - teacherFeature.get(paramName)) * LEARNING_COEF;
        cout << "learning: " << paramName << ' '
             << params.get(paramName, currentFeature.get(paramName)) << ' '
             << params.get(paramName, teacherFeature.get(paramName)) << ' '
             << currentFeature.get(paramName) << ' '
             << teacherFeature.get(paramName) << ' '
             << dT << endl;
        params.add(paramName, currentFeature.get(paramName), -dT);
        params.add(paramName, teacherFeature.get(paramName), dT);
    }

    {
        map<pair<PlanSparseFeatureParam, int>, int> diff;
        for (auto it = currentFeature.sparseFeatures().begin(); it != currentFeature.sparseFeatures().end(); ++it)
            diff[*it] += 1;
        for (auto it = teacherFeature.sparseFeatures().begin(); it != teacherFeature.sparseFeatures().end(); ++it)
            diff[*it] -= 1;

        cout << "*** " << diff.size() << endl;

        for (auto it = diff.begin(); it != diff.end(); ++it) {
            double dT = it->second * LEARNING_COEF;
            params.add(it->first.first, it->first.second, -dT);
        }
    }
}

class Learner {
public:
    Learner() : m_numLearn(0), m_numMatchedTeacher(0) {}

    void learn(EvaluationParams& params, const Gazer& gazer,
               const Field& currentField, const vector<KumiPuyo>& kumiPuyos,
               const Field& teacherField);

    int numLearn() const { return m_numLearn; }
    int numMatchedTeacher() const { return m_numMatchedTeacher; }
    double ratioMatchedTeacher() const { return 100 * numMatchedTeacher() / numLearn(); }
private:
    int m_numLearn;
    int m_numMatchedTeacher;
};

void Learner::learn(LearningEvaluationFeature* feature,
                    const CoreField& field, const KumipuyoSeq& kumipuyoSeq,
                    const Gazer& gazer, const Decision& teacherDecision)

{
    ++m_numLearn;
    cout << "learn" << endl;
    cout << currentField.debugOutput() << endl;
    for (const KumiPuyo& kumiPuyo : kumiPuyos) {
        cout << kumiPuyo.toString();
    }
    cout << endl;

    vector<Plan> plans;
    findAvailablePlans(currentField, 2, kumiPuyos, plans);
    if (plans.empty())
        return;

    // --- スコアを付ける
    vector<EvaluationFeature> features(plans.size());

    // TODO(mayah): We would like to use future instead of shared_future, however mac requires libc++ to do so.
    // If we use libc++, we have to recompile gtest as well...
    vector<boost::shared_future<void>> futures;
    futures.reserve(plans.size());

    for (size_t i = 0; i < plans.size(); ++i) {
        boost::packaged_task<void> pt(boost::bind(
                EvaluationFeatureCollector::collectFeatures,
                features[i],
                plans[i],
                AI::NUM_KEY_PUYOS,
                0,
                gazer));

        futures.push_back(pt.get_future());
        boost::thread th(boost::move(pt));
    }

    vector<pair<double, size_t> > scores(plans.size());
    for (size_t i = 0; i < plans.size(); ++i) {
        if (plans[i].isRensaPlan()) {
            scores[i] = make_pair(-1000000, i);
        } else {
            futures[i].wait();
            double score = features[i].calculateScore(params);
            scores[i] = make_pair(score, i);
        }
    }
    sort(scores.begin(), scores.end(), greater<pair<double, size_t> >());

    // --- 教師の盤面と、そのスコアを求める。
    size_t teacherIndex = scores.size();
    for (size_t i = 0; i < scores.size(); ++i) {
        if (plans[scores[i].second].isRensaPlan())
            continue;

        if (plans[scores[i].second].field() == teacherField) {
            teacherIndex = i;
            break;
        }
    }
    if (teacherIndex == scores.size()) {
        cerr << "We couldn't find teacher field. It might be a bug?" << endl;
        return;
    }

    // --- 教師の盤面と最もスコアが高い盤面が一緒であれば、何も更新する必要はない。
    if (teacherIndex == 0) {
        ++m_numMatchedTeacher;
        return;
    }

    // --- そうでなければ、スコア付けに問題があったということで、スコアを更新する。
    const EvaluationFeature& currentFeature = features[scores[0].second];
    const EvaluationFeature& teacherFeature = features[scores[teacherIndex].second];

    cout << "teacher decision = " << plans[scores[teacherIndex].second].decisionText() << " : " << scores[teacherIndex].first << endl;
    cout << teacherFeature.toString() << endl;
    cout << "our decision = " << plans[scores[0].second].decisionText() << " : " << scores[0].first << endl;
    cout << currentFeature.toString() << endl;

    updateParam(params, currentFeature, teacherFeature);
}

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    TsumoPossibility::initialize();

    LearningEvaluationFeature feature("feature.txt");
    Learner learner;
    Gazer gazer;

    CoreField field;
    KumipuyoSeq kumipuyoSeq = generateSequence();

    for (size_t i = 0; i + 2 < seq.size(); ++i) {
        KumipuyoSeq seq = kumipuyo.subsequence(i, 2);
        map<Decision, EvaluationFeature> m;
        Plan::iterateAvaiablePlans(field, seq, 2, [](const RefPlan& plan) {
            double score = Evaluator().eval(feature, plan, 1, gazer);
            if (score > maxScore)
                score = maxScore;
        });

        Decision teacherDecision;
        while (!teacherDecision.isValid()) {
            cout << "Decision ? " << endl;
            string str;
            getilne(cin, str);
            if (str == "q" || str = "quit" || str == "exit")
                break;

            int x, r;
            if (sscanf(str.c_str(), "%d %d", &x, &r) != 2)
                continue;

            teacherDecision = Decision(x, r);
        }

        if (!teacherDecision.isValid())
            break;

        learner.learn(field, seq, teacherDecision);
    }

    CHECK(feature.save("feature_learned.txt"));
    return 0;
}
