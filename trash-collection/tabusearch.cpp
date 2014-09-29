
#include <iostream>
#include <vector>
#include <algorithm>
#include "tabusearch.h"

void TabuSearch::dumpTabuList() const {
    std::map<const Move, int>::const_iterator it;

    std::cout << "TabuList at iteration: " << currentIteration << std::endl;
    for (it = TabuList.begin(); it!=TabuList.end(); ++it) {
        it->first.dump();
        std::cout << " - expires: " << it->second << std::endl;
    }
    std::cout << "--------------------------" << std::endl;
}


void TabuSearch::dumpStats() const {
    std::cout << "TabuList Stats at iteration: " << currentIteration << std::endl;
    std::cout << "   movesAdded: " << movesAdded << std::endl
              << "   movesChecked: " << movesChecked << std::endl
              << "   movesCheckedTabu: " << movesCheckedTabu << std::endl
              << "   bestUpdatedLastAt: " << bestUpdatedLastAt << std::endl
              << "   bestUpdatedCnt: " << bestUpdatedCnt << std::endl
              << "   cntInsApplied: " << cntInsApplied << std::endl
              << "   cntIntraSwApplied: " << cntIntraSwApplied << std::endl
              << "   cntInterSwApplied: " << cntInterSwApplied << std::endl
              ;
}


bool TabuSearch::isTabu(const Move& m) {
    std::map<const Move, int>::iterator it;

    ++movesChecked;  // this makes it non-const
    for (it = TabuList.begin(); it!=TabuList.end(); ++it) {
        if (it->second < currentIteration) continue;
        if (m.tabuEquiv(it->first)) {
            ++movesCheckedTabu;  // this makes it non-const
            return true;
        }
    }
    return false;
}


void TabuSearch::cleanExpired() {
    std::map<const Move, int>::iterator it;
    for (it = TabuList.begin(); it!=TabuList.end(); ++it)
        if (it->second < currentIteration)
            TabuList.erase(it);
}


void TabuSearch::makeTabu(const Move m) {
    TabuList[m] = currentIteration + tabuLength;
    ++movesAdded;
}


/*
    We use a sequential solving strategy for combining our three neighborhood
    move functions. This as know as a "token-ring" search. Given an initial
    state and a set of algorithms, it makes circularly a run at each algorithm,
    always starting from the best solution found by the previous one. The
    overall process stops either when a full round of the algorithms does not
    find an improvement or the time (aka: interation count) granted has elapsed.
    
    Each single algorithm stops when it does not improve the current best
    solution for a given number of iterations (ie: stagnation).

*/

void TabuSearch::search() {

    currentIteration = 0;

    bool madeChanges;
    do {
        madeChanges = doNeighborhoodMoves(Ins, 500)
                    | doNeighborhoodMoves(IntraSw, 300)
                    | doNeighborhoodMoves(InterSw, 300);
    }
    while (madeChanges and ++currentIteration < maxIteration);

}


/*
    For each of the individual move neighborhoods:
        doInsMoves, doIntraSwMoves, doInterSwMoves

    do {
        Generate the neighhood of feasible moves order by best to worst.
        Working through the neighborhood (best to worst) if the move is
        aspirational then apply the move otherwise if the move is not Tabu
        apply the move even if it makes the solution worse.
    } until there are not valid moves or stagnation 
    return madeChanges
*/

bool TabuSearch::doNeighborhoodMoves(Neighborhoods whichNeighborhood, int maxStagnation) {
    bool madeMove;
    bool loopMadeMove;
    int stagnationCnt = 0;
    madeMove = false;

    do {
        loopMadeMove = false;

        // generate the a move neighborhood based on the currentSolution
        std::vector<Move> neighborhood;
        switch (whichNeighborhood) {
            case Ins:
                currentSolution.getInsNeighborhood(neighborhood);
                break;
            case IntraSw:
                currentSolution.getIntraSwNeighborhood(neighborhood);
                break;
            case InterSw:
                currentSolution.getInterSwNeighborhood(neighborhood);
                break;
        }

        // and sort it
        std::sort(neighborhood.begin(), neighborhood.end(), Move::bySavings);

        // take the best move that we may apply and apply it, if any
        for (std::vector<Move>::iterator it=neighborhood.begin();
                it!=neighborhood.end(); ++it) {
            if (currentSolution.getCost() - it->getsavings() < bestSolutionCost) {
                currentSolution.applyMove(*it);
                bestSolution = currentSolution;
                bestSolutionCost = bestSolution.getCost();
                makeTabu(*it);
                loopMadeMove = true;
                stagnationCnt = 0;
                bestUpdatedLastAt = currentIteration;
                ++bestUpdatedCnt;
                // update stats
                switch (whichNeighborhood) {
                    case Ins:     ++cntInsApplied;    break;
                    case IntraSw: ++cntIntraSwApplied; break;
                    case InterSw: ++cntInterSwApplied; break;
                }
            }
            else if (! isTabu(*it)) {
                currentSolution.applyMove(*it);
                makeTabu(*it);
                loopMadeMove = true;
                // update stats
                switch (whichNeighborhood) {
                    case Ins:     ++cntInsApplied;    break;
                    case IntraSw: ++cntIntraSwApplied; break;
                    case InterSw: ++cntInterSwApplied; break;
                }
            }
        }
        madeMove = madeMove or loopMadeMove;
        ++stagnationCnt;
    }
    while (madeMove and stagnationCnt < maxStagnation);

    return madeMove;
}


