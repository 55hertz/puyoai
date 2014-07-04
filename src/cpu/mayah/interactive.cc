#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "core/algorithm/puyo_possibility.h"
#include "core/algorithm/plan.h"
#include "core/algorithm/rensa_info.h"
#include "core/algorithm/rensa_detector.h"
#include "core/client/connector/drop_decision.h"
#include "core/field/core_field.h"
#include "core/field/field_pretty_printer.h"
#include "core/frame_data.h"
#include "core/kumipuyo.h"
#include "core/sequence_generator.h"
#include "core/state.h"

#include "evaluator.h"
#include "mayah_ai.h"
#include "util.h"

DEFINE_string(seq, "", "initial puyo sequence");

using namespace std;

class InteractiveAI : public MayahAI {
public:
    using MayahAI::gameWillBegin;
    using MayahAI::think;
    using MayahAI::enemyNext2Appeared;

    CollectedFeature makeCollectedFeature(int frameId, const CoreField& field, int numKeyPuyos, const Plan& plan) const
    {
        RefPlan refPlan(plan.field(), plan.decisions(), plan.rensaResult(), plan.numChigiri(), plan.initiatingFrames());
        return Evaluator(*featureParameter_).evalWithCollectingFeature(refPlan, field, frameId, numKeyPuyos, gazer_);
    }

    Plan thinkPlanOnly(int frameId, const CoreField& field, const KumipuyoSeq& kumipuyoSeq,
                       int depth, const vector<Decision>& decisions) const
    {
        Plan result;
        Plan::iterateAvailablePlans(field, kumipuyoSeq, depth,
                                    [this, frameId, &field, &decisions, &result](const RefPlan& plan) {
            if (plan.decisions() != decisions)
                return;
            result = plan.toPlan();
        });

        return result;
    }
};

// TODO(mayah): Implement with GUI!
int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    TsumoPossibility::initialize();

    InteractiveAI ai;

    CoreField field;
    KumipuyoSeq seq = generateSequence();

    if (FLAGS_seq != "") {
        for (size_t i = 0; i < FLAGS_seq.size(); ++i) {
            PuyoColor c = toPuyoColor(FLAGS_seq[i]);
            CHECK(isNormalColor(c));
            if (i % 2 == 0)
                seq.setAxis(i / 2, c);
            else
                seq.setChild(i / 2, c);
        }
    }

    for (int i = 0; i + 1 < seq.size(); ++i) {
        int frameId = 2 + i; // frameId 1 will be used for initializing now. Let's avoid it.

        FrameData fd;
        fd.id = frameId;
        fd.valid = true;

        // Set up enemy field.
        // Make enemy will fire his large rensa.
        fd.playerFrameData[1].field = CoreField(
            "500065"
            "400066"
            "545645"
            "456455"
            "545646"
            "545646"
            "564564"
            "456456"
            "456456"
            "456456");
        fd.playerFrameData[1].kumipuyoSeq = KumipuyoSeq("666666");

        // Invoke Gazer.
        {
            double t1 = now();
            ai.enemyNext2Appeared(fd);
            double t2 = now();
            cout << "gazer time = " << (t2 - t1) << endl;
        }

        FieldPrettyPrinter::print(field, seq.subsequence(i, 2));

        double t1 = now();
        Plan plan = ai.thinkPlan(frameId, field, KumipuyoSeq { seq.get(i), seq.get(i + 1) },
                                 MayahAI::DEFAULT_DEPTH, MayahAI::DEFAULT_NUM_KEY_PUYOS);
        double t2 = now();
        if (plan.decisions().empty())
            cout << "No decision";
        else
            cout << plan.decision(0).toString() << "-" << plan.decision(1).toString();
        cout << " time = " << ((t2 - t1) * 1000) << " [ms]" << endl;

        Plan aiPlan = ai.thinkPlan(frameId, field, KumipuyoSeq { seq.get(i), seq.get(i + 1) },
                                   MayahAI::DEFAULT_DEPTH, MayahAI::DEFAULT_NUM_KEY_PUYOS);

        // Waits for user enter.
        while (true) {
            string str;
            cout << "command? ";
            getline(cin, str);

            if (str == "")
                break;
            if (str == "s") {
                CollectedFeature cf = ai.makeCollectedFeature(frameId, field, MayahAI::DEFAULT_NUM_KEY_PUYOS, aiPlan);
                cout << cf.toString() << endl;
                continue;
            }

            int x1, r1, x2, r2;
            if (sscanf(str.c_str(), "%d %d %d %d", &x1, &r1, &x2, &r2) == 4) {
                vector<Decision> decisions {
                    Decision(x1, r1),
                    Decision(x2, r2)
                };
                Plan plan = ai.thinkPlanOnly(frameId, field, KumipuyoSeq { seq.get(i), seq.get(i + 1) },
                                             MayahAI::DEFAULT_DEPTH, decisions);

                CollectedFeature mycf = ai.makeCollectedFeature(frameId, field, MayahAI::DEFAULT_NUM_KEY_PUYOS, plan);
                CollectedFeature aicf = ai.makeCollectedFeature(frameId, field, MayahAI::DEFAULT_NUM_KEY_PUYOS, aiPlan);
                cout << mycf.toStringComparingWith(aicf) << endl;
            }
        }

        field.dropKumipuyo(aiPlan.decisions().front(), seq.get(i));
        field.simulate();
    }

    return 0;
}
