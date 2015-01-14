#include "core/puyo_controller.h"

#include <gtest/gtest.h>
#include <vector>

#include "core/core_field.h"
#include "core/decision.h"
#include "core/kumipuyo.h"
#include "core/plain_field.h"

using namespace std;

TEST(PuyoControllerMoveTest, moveKumipuyoWithOnlyArrowKey)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(3, 12, 0));
    bool downAccepted = false;

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(4, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);

    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::LEFT), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 3, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);

    // TODO(mayah): Should we accept Key::DOWN here?
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::DOWN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(0, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_TRUE(downAccepted);

    downAccepted = false;
    PuyoController::moveKumipuyo(f, KeySet(Key::DOWN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 11, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(0, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_TRUE(downAccepted);

    mks.pos = KumipuyoPos(3, 1, 0);
    mks.grounding = false;
    mks.restFramesForFreefall = FRAMES_FREE_FALL;

    downAccepted = false;
    PuyoController::moveKumipuyo(f, KeySet(Key::DOWN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 1, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL, mks.restFramesForFreefall);
    EXPECT_TRUE(mks.grounding);
    EXPECT_FALSE(mks.grounded);
    EXPECT_TRUE(downAccepted);

    downAccepted = false;
    PuyoController::moveKumipuyo(f, KeySet(Key::DOWN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 1, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(0, mks.restFramesForFreefall);
    EXPECT_TRUE(mks.grounded);
    EXPECT_TRUE(downAccepted);
}

TEST(PuyoControllerMoveTest, moveKumipuyoWithOnlyArrowKey_Right)
{
    CoreField f;
    bool downAccepted = false;

    {
        MovingKumipuyoState mks(KumipuyoPos(3, 12, 0));
        PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(4, 12, 0), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
    {
        MovingKumipuyoState mks(KumipuyoPos(3, 12, 1));
        PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(4, 12, 1), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
    {
        MovingKumipuyoState mks(KumipuyoPos(3, 11, 2));
        PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(4, 11, 2), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
    {
        MovingKumipuyoState mks(KumipuyoPos(3, 12, 3));
        PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(4, 12, 3), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
}

TEST(PuyoControllerMoveTest, moveKumipuyoWithOnlyArrowKey_Left)
{
    CoreField f;
    bool downAccepted = false;

    {
        MovingKumipuyoState mks(KumipuyoPos(3, 12, 0));
        PuyoController::moveKumipuyo(f, KeySet(Key::LEFT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(2, 12, 0), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
    {
        MovingKumipuyoState mks(KumipuyoPos(3, 12, 1));
        PuyoController::moveKumipuyo(f, KeySet(Key::LEFT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(2, 12, 1), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
    {
        MovingKumipuyoState mks(KumipuyoPos(3, 11, 2));
        PuyoController::moveKumipuyo(f, KeySet(Key::LEFT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(2, 11, 2), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
    {
        MovingKumipuyoState mks(KumipuyoPos(3, 12, 3));
        PuyoController::moveKumipuyo(f, KeySet(Key::LEFT), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(2, 12, 3), mks.pos);
        EXPECT_EQ(0, mks.restFramesTurnProhibited);
        EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
        EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
        EXPECT_FALSE(mks.grounded);
    }
}

TEST(PuyoControllerMoveTest, moveKumipuyoFreefall)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(3, 12, 0));
    mks.restFramesForFreefall = 2;
    bool downAccepted = false;

    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(1, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);

    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 11, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);

    mks.pos = KumipuyoPos(3, 1, 0);
    mks.grounding = false;
    mks.restFramesForFreefall = FRAMES_FREE_FALL / 2 + 1;

    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 1, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_TRUE(mks.grounding);
    EXPECT_FALSE(downAccepted);

    mks.restFramesForFreefall = 1;

    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 1, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_TRUE(mks.grounded);
    EXPECT_FALSE(downAccepted);
}

TEST(PuyoControllerMoveTest, moveKumipuyoWithTurnKey)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(3, 12, 0));
    bool downAccepted = false;

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 1), mks.pos);
    EXPECT_EQ(PuyoController::FRAMES_CONTINUOUS_TURN_PROHIBITED, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);

    // RIGHT_TURN should be ignored during mks.resetFrames.TurnProhibited > 0.
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 1), mks.pos);
    EXPECT_EQ(PuyoController::FRAMES_CONTINUOUS_TURN_PROHIBITED - 1, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 2, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);

    while (mks.restFramesTurnProhibited > 0) {
        PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    }

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 2), mks.pos);
}

TEST(PuyoControllerMoveTest, moveKumipuyoMultipleKeys)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(1, 12, 0));
    bool downAccepted = false;

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT, Key::LEFT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 3), mks.pos);
    EXPECT_EQ(PuyoController::FRAMES_CONTINUOUS_TURN_PROHIBITED, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);
}

TEST(PuyoControllerMoveTest, wontGrounding)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(3, 2, 1));
    mks.restFramesForFreefall = FRAMES_FREE_FALL;
    bool downAccepted = false;

    EXPECT_EQ(0, mks.numGrounded);
    EXPECT_FALSE(mks.grounding);

    // In this case, it won't move to the grounding state.
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);

    EXPECT_EQ(1, mks.pos.childY());
    EXPECT_EQ(0, mks.numGrounded);
    EXPECT_FALSE(mks.grounding);
}

TEST(PuyoControllerMoveTest, grounding)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(3, 2, 1));
    mks.restFramesForFreefall = FRAMES_FREE_FALL / 2;
    bool downAccepted = false;

    EXPECT_EQ(0, mks.numGrounded);
    EXPECT_FALSE(mks.grounding);

    // In this case, it should move to the grounding state.
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);

    EXPECT_EQ(1, mks.pos.childY());
    EXPECT_EQ(1, mks.numGrounded);
    EXPECT_TRUE(mks.grounding);
}

TEST(PuyoControllerMoveTest, fixAfterEightGrouding)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(3, 2, 1));
    bool downAccepted = false;

    mks.restFramesForFreefall = FRAMES_FREE_FALL / 2;

    EXPECT_EQ(0, mks.numGrounded);
    EXPECT_FALSE(mks.grounding);

    for (int i = 0; i < 7; ++i) {
        PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
        EXPECT_EQ(1, mks.pos.childY());
        EXPECT_EQ(i + 1, mks.numGrounded) << i;
        EXPECT_TRUE(mks.grounding);

        PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
        EXPECT_EQ(1, mks.pos.childY());
        EXPECT_EQ(i + 1, mks.numGrounded) << i;
        EXPECT_TRUE(mks.grounding);

        PuyoController::moveKumipuyo(f, KeySet(Key::LEFT_TURN), &mks, &downAccepted);
        EXPECT_EQ(2, mks.pos.childY());
        EXPECT_EQ(i + 1, mks.numGrounded) << i;
        EXPECT_FALSE(mks.grounding);

        PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
        EXPECT_EQ(2, mks.pos.childY());
        EXPECT_EQ(i + 1, mks.numGrounded) << i;
        EXPECT_FALSE(mks.grounding);
    }

    // After 8th grounding, kumipuyo should be fixed.
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(1, mks.pos.childY());
    EXPECT_EQ(8, mks.numGrounded);
    EXPECT_TRUE(mks.grounded);
}

TEST(PuyoControllerMoveTest, moveKumipuyoWithLiftingAxis_RightTurn)
{
    CoreField f;
    bool downAccepted = false;

    MovingKumipuyoState mks(KumipuyoPos(3, 1, 1));
    mks.grounding = true;

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 2), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 3), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 0), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 1, 1), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 2), mks.pos);
}

TEST(PuyoControllerMoveTest, moveKumipuyoWithLiftingAxis_LeftTurn)
{
    CoreField f;
    bool downAccepted = false;

    MovingKumipuyoState mks(KumipuyoPos(3, 1, 3));
    mks.grounding = true;

    PuyoController::moveKumipuyo(f, KeySet(Key::LEFT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 2), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::LEFT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 1), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::LEFT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 0), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::LEFT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 1, 3), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::LEFT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 2, 2), mks.pos);
}

TEST(PuyoControllerMoveTest, moveKumipuyoWithLiftingAxis12)
{
    CoreField f(
        "OOOOOO" // 11
        "OOOOOO"
        "OOOOOO"
        "OOOOOO" // 8
        "OOOOOO"
        "OOOOOO"
        "OOOOOO"
        "OOOOOO" // 4
        "OOOOOO"
        "OOOOOO"
        "OOOOOO");

    bool downAccepted = false;

    MovingKumipuyoState mks(KumipuyoPos(3, 12, 1));
    mks.grounding = true;

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 13, 2), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 13, 3), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 13, 0), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 1), mks.pos);
    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 13, 2), mks.pos);
}

TEST(PuyoControllerMoveTest, moveKumipuyoWithLiftingAxis13)
{
    CoreField f(
        "   OOO" // 12
        "OOOOOO" // 11
        "OOOOOO"
        "OOOOOO"
        "OOOOOO" // 8
        "OOOOOO"
        "OOOOOO"
        "OOOOOO"
        "OOOOOO" // 4
        "OOOOOO"
        "OOOOOO"
        "OOOOOO"
        );
    bool downAccepted = false;

    {
        MovingKumipuyoState mks(KumipuyoPos(5, 13, 1));
        mks.grounding = true;

        PuyoController::moveKumipuyo(f, KeySet(Key::LEFT_TURN), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(5, 13, 0), mks.pos);
    }

    {
        // Axis cannot be moved to the 14th line.
        MovingKumipuyoState mks(KumipuyoPos(5, 13, 1));
        PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
        EXPECT_EQ(KumipuyoPos(5, 13, 1), mks.pos);
    }
}

TEST(PuyoControllerMoveTest, moveKumipuyoQuickTurn)
{
    CoreField f(
        " O O  " // 12
        " O O  "
        " O O  "
        " O O  "
        " O O  " // 8
        " O O  "
        " O O  "
        " O O  "
        " O O  " // 4
        " O O  "
        " O O  "
        " O O  ");

    MovingKumipuyoState mks(KumipuyoPos(3, 12, 0));
    bool downAccepted = false;

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 0), mks.pos);
    EXPECT_EQ(PuyoController::FRAMES_CONTINUOUS_TURN_PROHIBITED, mks.restFramesTurnProhibited);
    EXPECT_EQ(FRAMES_QUICKTURN, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);

    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 12, 0), mks.pos);
    EXPECT_EQ(PuyoController::FRAMES_CONTINUOUS_TURN_PROHIBITED - 1, mks.restFramesTurnProhibited);
    EXPECT_EQ(FRAMES_QUICKTURN - 1, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 2, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(3, 13, 2), mks.pos);
    EXPECT_EQ(PuyoController::FRAMES_CONTINUOUS_TURN_PROHIBITED, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL / 2, mks.restFramesForFreefall); // After lifted, frames free fall should be an initial value / 2.
    EXPECT_FALSE(mks.grounded);
    EXPECT_FALSE(downAccepted);

    PuyoController::moveKumipuyo(f, KeySet(), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT_TURN), &mks, &downAccepted);
    PuyoController::moveKumipuyo(f, KeySet(Key::DOWN), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(4, 13, 0), mks.pos);
}

TEST(PuyoControllerMoveTest, moveForWii)
{
    CoreField f;
    MovingKumipuyoState mks(KumipuyoPos(3, 12, 0));
    bool downAccepted = false;

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(4, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(1, mks.restFramesArrowProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 1, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);

    // This should be ignored.
    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(4, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesArrowProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 2, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);

    PuyoController::moveKumipuyo(f, KeySet(Key::RIGHT), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(5, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(1, mks.restFramesArrowProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 3, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);

    // This should be ignored.
    PuyoController::moveKumipuyo(f, KeySet(Key::LEFT), &mks, &downAccepted);
    EXPECT_EQ(KumipuyoPos(5, 12, 0), mks.pos);
    EXPECT_EQ(0, mks.restFramesTurnProhibited);
    EXPECT_EQ(0, mks.restFramesArrowProhibited);
    EXPECT_EQ(0, mks.restFramesToAcceptQuickTurn);
    EXPECT_EQ(FRAMES_FREE_FALL - 4, mks.restFramesForFreefall);
    EXPECT_FALSE(mks.grounded);
}
