#include "mayah_ai.h"

#include <gflags/gflags.h>

#include "core/algorithm/plan.h"
#include "core/algorithm/puyo_possibility.h"
#include "core/client/connector/drop_decision.h"
#include "core/frame_data.h"

#include "feature_parameter.h"
#include "evaluator.h"
#include "gazer.h"

DECLARE_string(feature);

using namespace std;

MayahAI::MayahAI() :
    AI("mayah")
{
    featureParameter_.reset(new FeatureParameter(FLAGS_feature));
    LOG(INFO) << featureParameter_->toString();
}

MayahAI::~MayahAI()
{
}

void MayahAI::gameWillBegin(const FrameData& frameData)
{
    gazer_.initializeWith(frameData.id);
}

void MayahAI::gameHasEnded(const FrameData&)
{
}

DropDecision MayahAI::think(int frameId, const PlainField& plainField, const Kumipuyo& next1, const Kumipuyo& next2)
{
    CoreField f(plainField);
    KumipuyoSeq kumipuyoSeq { next1, next2 };
    return thinkInternal(frameId, f, kumipuyoSeq, false);
}

DropDecision MayahAI::thinkFast(int frameId, const PlainField& plainField, const Kumipuyo& next1, const Kumipuyo& next2)
{
    CoreField f(plainField);
    KumipuyoSeq kumipuyoSeq { next1, next2 };
    return thinkInternal(frameId, f, kumipuyoSeq, true);
}

DropDecision MayahAI::thinkInternal(int frameId, const CoreField& field, const KumipuyoSeq& kumipuyoSeq, bool fast)
{
    LOG(INFO) << "\n" << field.debugOutput() << "\n" << kumipuyoSeq.toString();

    double bestScore = -100000000.0;
    DropDecision dropDecision;
    Plan::iterateAvailablePlans(field, kumipuyoSeq, 2,
                                [this, frameId, fast, &field, &bestScore, &dropDecision](const RefPlan& plan) {
        double score = Evaluator(*featureParameter_).eval(plan, field, frameId, fast, gazer_);
        if (bestScore < score) {
            bestScore = score;
            dropDecision = DropDecision(plan.decisions().front(), "");
        }
    });

    LOG(INFO) << "Decided : " << dropDecision.decision().toString();
    return dropDecision;
}

void MayahAI::enemyGrounded(const FrameData& frameData)
{
    gazer_.setId(frameData.id);

    // --- Check if Rensa starts.
    CoreField field(frameData.enemyPlayerFrameData().field);
    field.forceDrop();

    RensaResult rensaResult = field.simulate();

    if (rensaResult.chains > 0)
        gazer_.setOngoingRensa(OngoingRensaInfo(rensaResult, frameData.id + rensaResult.frames));
    else
        gazer_.setRensaIsOngoing(false);
}

void MayahAI::enemyNext2Appeared(const FrameData& frameData)
{
    int currentFrameId = frameData.id;

    gazer_.setId(currentFrameId);
    gazer_.updateFeasibleRensas(frameData.enemyPlayerFrameData().field, frameData.enemyPlayerFrameData().kumipuyoSeq);
    gazer_.updatePossibleRensas(frameData.enemyPlayerFrameData().field, frameData.enemyPlayerFrameData().kumipuyoSeq);

    LOG(INFO) << "Possible rensa infos : ";
    for (auto it = gazer_.possibleRensaInfos().begin(); it != gazer_.possibleRensaInfos().end(); ++it)
        LOG(INFO) << it->toString();
    LOG(INFO) << "Feasible rensa infos : ";
    for (auto it = gazer_.feasibleRensaInfos().begin(); it != gazer_.feasibleRensaInfos().end(); ++it)
        LOG(INFO) << it->toString();
}
