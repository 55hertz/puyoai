#ifndef EVALUATION_FEATURE_COLLECTOR_H_
#define EVALUATION_FEATURE_COLLECTOR_H_

#include <map>
#include <vector>

#include "evaluation_feature.h"
#include "pattern_book.h"
#include "score_collector.h"

class ColumnPuyoList;
class CoreField;
class GazeResult;
class PuyoSet;
class RefPlan;
class RensaChainTrackResult;

struct PlayerState;
struct RensaResult;

class EvaluatorBase {
protected:
    EvaluatorBase(const PatternBook& patternBook) :
        patternBook_(patternBook) {}

    const PatternBook& patternBook() const { return patternBook_; }

private:
    const PatternBook& patternBook_;
};

class PreEvalResult {
public:
    PreEvalResult() {}

    const std::vector<int>& matchablePatternIds() const { return matchablePatternIds_; }

    std::vector<int>* mutableMatchablePatternIds() { return &matchablePatternIds_; }

private:
    std::vector<int> matchablePatternIds_;
};

class PreEvaluator : public EvaluatorBase {
public:
    PreEvaluator(const PatternBook& patternBook) :
        EvaluatorBase(patternBook) {}

    PreEvalResult preEval(const CoreField& currentField);
};

class MidEvalResult {
public:
    void add(EvaluationFeatureKey key, double value)
    {
        collectedFeatures_[key] = value;
    }

    double feature(EvaluationFeatureKey key) const
    {
        auto it = collectedFeatures_.find(key);
        if (it == collectedFeatures_.end())
            return 0;

        return it->second;
    }

    const std::map<EvaluationFeatureKey, double>& collectedFeatures() const { return collectedFeatures_; }

private:
    std::map<EvaluationFeatureKey, double> collectedFeatures_;
};

class MidEvaluator : public EvaluatorBase {
public:
    MidEvaluator(const PatternBook& patternBook) :
        EvaluatorBase(patternBook) {}

    MidEvalResult eval(const RefPlan&, const CoreField& currentField, double score);
};

class EvalResult {
public:
    constexpr EvalResult(double score, int maxVirtualScore) : score_(score), maxVirtualScore_(maxVirtualScore) {}

    double score() const { return score_; }
    int maxVirtualScore() const { return maxVirtualScore_; }

private:
    double score_;
    int maxVirtualScore_;
};

template<typename ScoreCollector>
class RensaEvaluator : public EvaluatorBase {
public:
    // Don't take ownership of |sc|.
    RensaEvaluator(const PatternBook& patternBook,
                   ScoreCollector* sc) :
        EvaluatorBase(patternBook),
        sc_(sc) {}

    void evalPatternScore(const ColumnPuyoList& puyosToComplement, double patternScore);
    void evalRensaScore(double score, double virtualScore);
    void evalRensaChainFeature(const RensaResult&, const PuyoSet&);
    void evalRensaGarbage(const CoreField& fieldAfterRensa);
    void evalFirePointTabooFeature(const CoreField&, const RensaChainTrackResult&);
    void evalRensaIgnitionHeightFeature(const CoreField&, const RensaChainTrackResult&, bool enemyHasZenkeshi);
    void evalRensaConnectionFeature(const CoreField& fieldAfterDrop);
    void evalRensaRidgeHeight(const CoreField&);
    void evalRensaValleyDepth(const CoreField&);
    void evalRensaFieldUShape(const CoreField&, bool enemyHasZenkeshi);
    void evalComplementationBias(const ColumnPuyoList&);
    void evalRensaStrategy(const RefPlan&, const RensaResult&, const ColumnPuyoList&,
                           int currentFrameId, const PlayerState& me, const PlayerState& enemy);
private:
    ScoreCollector* sc_;
};

template<typename ScoreCollector>
class Evaluator : public EvaluatorBase {
public:
    // Don't take ownership of |sc|.
    Evaluator(const PatternBook& patternBook, ScoreCollector* sc) :
        EvaluatorBase(patternBook),
        sc_(sc) {}

    void eval(const RefPlan&, int currentFrameId, int maxIteration,
              const PlayerState& me, const PlayerState& enemy,
              const PreEvalResult&, const MidEvalResult&, const GazeResult&);

    // ----------------------------------------------------------------------

    bool evalStrategy(const RefPlan&, int currentFrameId,
                      const PlayerState& me, const PlayerState& enemy, const GazeResult&,
                      const MidEvalResult&);

    void evalFrameFeature(int totalFrames, int numChigiri);
    void evalRestrictedConnectionHorizontalFeature(const CoreField&);
    void evalThirdColumnHeightFeature(const CoreField&);
    void evalValleyDepth(const CoreField&);
    void evalRidgeHeight(const CoreField&);
    void evalFieldUShape(const CoreField&, bool enemyHasZenkeshi);
    void evalUnreachableSpace(const CoreField&);
    void evalFallenOjama(int fallenOjama);

    void evalMidEval(const MidEvalResult&);

    void evalConnection(const CoreField&);
    void evalCountPuyoFeature(const CoreField&);

private:
    CollectedCoef calculateDefaultCoef(const PlayerState& me, const PlayerState& enemy) const;

    ScoreCollector* sc_;
};

#endif
