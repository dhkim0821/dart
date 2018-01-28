/*
 * Copyright (c) 2011-2017, The DART development contributors
 * All rights reserved.
 *
 * The list of contributors can be found at:
 *   https://github.com/dartsim/dart/blob/master/LICENSE
 *
 * This file is provided under the following "BSD-style" License:
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DART_CONSTRAINT_PGSBOXEDLCPSOLVER_HPP_
#define DART_CONSTRAINT_PGSBOXEDLCPSOLVER_HPP_

#include <vector>
#include "dart/constraint/BoxedLcpSolver.hpp"

namespace dart {
namespace constraint {

/// Implementation of projected Gauss-Seidel (PGS) LCP solver.
class PgsBoxedLcpSolver : public BoxedLcpSolver
{
public:
  struct Option
  {
    int mMaxIteration;
    double mDeltaXThreshold;
    double mRelativeDeltaXTolerance;
    double mEpsilonForDivision;
    bool mRandomizeConstraintOrder;

    Option(
        int maxIteration = 30,
        double deltaXTolerance = 1e-6,
        double relativeDeltaXTolerance = 1e-3,
        double epsilonForDivision = 1e-9,
        bool randomizeConstraintOrder = false);
  };

  // Documentation inherited.
  void solve(
      int n,
      double* A,
      double* x,
      double* b,
      int nub,
      double* lo,
      double* hi,
      int* findex) override;

  void solve(
      const Eigen::MatrixXd& A,
      Eigen::VectorXd& x,
      const Eigen::VectorXd& b,
      int nub,
      const Eigen::VectorXd& lo,
      const Eigen::VectorXd& hi,
      const Eigen::VectorXi& frictionIndex);

  void solve(
      const Eigen::MatrixXd& A,
      Eigen::VectorXd& x,
      const Eigen::VectorXd& b,
      int nub,
      const Eigen::VectorXd& lo,
      const Eigen::VectorXd& hi) override;

  // Documentation inherited.
  bool canSolve(int n, const double* A) override;

  /// Sets options
  void setOption(const Option& option);

  /// Returns options.
  const Option& getOption() const;

protected:
  void singleIterationForNormalizedA(
      int nskip,
      int* order,
      int n,
      int n_new,
      const double* normalizedA,
      double* x,
      double* b,
      double* lo,
      double* hi,
      int* findex,
      bool& sentinel);

  void sweepForward(
      const Eigen::MatrixXd& A, Eigen::VectorXd& x, const Eigen::VectorXd& b)
  {
    mCacheZ = -b;
    mCacheZ.noalias() -= A.triangularView<Eigen::StrictlyUpper>() * x;
    x.noalias() = A.triangularView<Eigen::Lower>().solve(mCacheZ);
  }

  void sweepForwardNormalized(
      const Eigen::MatrixXd& A, Eigen::VectorXd& x, const Eigen::VectorXd& b)
  {
    mCacheZ = -b;
    mCacheZ.noalias() -= A.triangularView<Eigen::StrictlyUpper>() * x;
    x.noalias() = A.triangularView<Eigen::UnitLower>().solve(mCacheZ);
  }

  void sweepBackward(
      const Eigen::MatrixXd& A, Eigen::VectorXd& x, const Eigen::VectorXd& b)
  {
    mCacheZ = -b;
    mCacheZ.noalias() -= A.triangularView<Eigen::StrictlyLower>() * x;
    x.noalias() = A.triangularView<Eigen::Upper>().solve(mCacheZ);
  }

  void sweepBackwardNormalized(
      const Eigen::MatrixXd& A, Eigen::VectorXd& x, const Eigen::VectorXd& b)
  {
    mCacheZ = -b;
    mCacheZ.noalias() -= A.triangularView<Eigen::StrictlyLower>() * x;
    x.noalias() = A.triangularView<Eigen::UnitUpper>().solve(mCacheZ);
  }

  Option mOption;

  mutable std::vector<int> mCacheOrder;
  mutable std::vector<double> mCacheD;
  mutable Eigen::VectorXd mCachedNormalizedA;
  mutable Eigen::MatrixXd mCachedNormalizedB;
  mutable Eigen::VectorXd mCacheZ;
  mutable Eigen::VectorXd mCacheOldX;
};

} // namespace constraint
} // namespace dart

#endif // DART_CONSTRAINT_PGSBOXEDLCPSOLVER_HPP_
