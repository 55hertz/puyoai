#include <gflags/gflags.h>
#include <glog/logging.h>

#include "core/algorithm/plan.h"
#include "core/client/ai/ai.h"
#include "core/core_field.h"

DEFINE_bool(right_turn, false, "Use right turn");

class SampleSuicideAI : public AI {
public:
    SampleSuicideAI(int argc, char* argv[]) : AI(argc, argv, "sample-suicide") {}
    virtual ~SampleSuicideAI() {}

    virtual DropDecision think(int frameId, const CoreField& f, const KumipuyoSeq& seq,
                               const AdditionalThoughtInfo& info, bool fast) override
    {
        UNUSED_VARIABLE(frameId);
        UNUSED_VARIABLE(seq);
        UNUSED_VARIABLE(info);
        UNUSED_VARIABLE(fast);

        Decision d;
        if (FLAGS_right_turn) {
            d = Decision(3, 1);
        } else {
            d = f.height(3) > 6 ? Decision(3, 0) : Decision(3, 2);
        }
        return DropDecision(d);
    }
};

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    SampleSuicideAI(argc, argv).runLoop();

    return 0;
}
