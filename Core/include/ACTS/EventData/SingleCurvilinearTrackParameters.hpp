// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTS_SINGLECURVILINEARTRACKPARAMETERS_H
#define ACTS_SINGLECURVILINEARTRACKPARAMETERS_H 1

// STL include(s)
#include <memory>

// ACTS includes
#include "ACTS/EventData/SingleTrackParameters.hpp"
#include "ACTS/Surfaces/PlaneSurface.hpp"

namespace Acts
{
  template<typename ChargePolicy>
  class SingleCurvilinearTrackParameters : public SingleTrackParameters<ChargePolicy>
  {
  public:
    typedef typename SingleTrackParameters<ChargePolicy>::CovPtr_t  CovPtr_t;  ///< type of covariance matrix

    template<typename T = ChargePolicy,std::enable_if_t<std::is_same<T,ChargedPolicy>::value,int> = 0>
    SingleCurvilinearTrackParameters(CovPtr_t cov,
                                     const ActsVectorD<3>& position,
                                     const ActsVectorD<3>& momentum,
                                     double dCharge):
      SingleTrackParameters<ChargePolicy>(std::move(cov),
                                          detail::coordinate_transformation::global2curvilinear(position,momentum,dCharge),
                                          position,
                                          momentum)
    {}

    template<typename T = ChargePolicy,std::enable_if_t<std::is_same<T,NeutralPolicy>::value,int> = 0>
    SingleCurvilinearTrackParameters(CovPtr_t cov,
                                     const ActsVectorD<3>& position,
                                     const ActsVectorD<3>& momentum):
      SingleTrackParameters<ChargePolicy>(std::move(cov),
                                          detail::coordinate_transformation::global2curvilinear(position,momentum,0),
                                          position,
                                          momentum)
    {}

    /**
     * @brief copy constructor
     */
    SingleCurvilinearTrackParameters(const SingleCurvilinearTrackParameters<ChargePolicy>& copy):
      SingleTrackParameters<ChargePolicy>(copy),
      m_upSurface()
    {}

    /**
     * @brief move constructor
     */
    SingleCurvilinearTrackParameters(SingleCurvilinearTrackParameters<ChargePolicy>&& copy):
      SingleTrackParameters<ChargePolicy>(std::move(copy)),
      m_upSurface(std::move(copy.m_upSurface))
    {}

    virtual ~SingleCurvilinearTrackParameters() = default;

    /**
     * @brief copy assignment operator
     */
    SingleCurvilinearTrackParameters<ChargePolicy>& operator=(const SingleCurvilinearTrackParameters<ChargePolicy>& rhs)
    {
      // check for self-assignment
      if(this != &rhs)
      {
        SingleTrackParameters<ChargePolicy>::operator=(rhs);
        m_upSurface.reset();
      }

      return *this;
    }

    /**
     * @brief move assignment operator
     */
    SingleCurvilinearTrackParameters<ChargePolicy>& operator=(SingleCurvilinearTrackParameters<ChargePolicy>&& rhs)
    {
      // check for self-assignment
      if(this != &rhs)
      {
        SingleTrackParameters<ChargePolicy>::operator=(std::move(rhs));
        m_upSurface = std::move(rhs.m_upSurface);
      }

      return *this;
    }

    virtual SingleTrackParameters<ChargePolicy>* clone() const override
    {
      return new SingleCurvilinearTrackParameters<ChargePolicy>(*this);
    }

    template<ParID_t par,std::enable_if_t<not std::is_same<typename par_type<par>::type,local_parameter>::value,int> = 0>
    void set(ParValue_t newValue)
    {
      this->getParameterSet().template setParameter<par>(newValue);
      this->updateGlobalCoordinates(typename par_type<par>::type());
    }

    virtual const Surface& associatedSurface() const final
    {
      m_upSurface.reset(new PlaneSurface(this->position(),this->momentum()));

      return *m_upSurface;
    }

  private:
    mutable std::unique_ptr<PlaneSurface> m_upSurface;
  };
}  // end of namespace Acts

#endif // ACTS_SINGLECURVILINEARTRACKPARAMETERS_H
