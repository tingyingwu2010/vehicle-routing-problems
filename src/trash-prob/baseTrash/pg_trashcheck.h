/*VRP*********************************************************************
 *
 * vehicle routing problems
 *      A collection of C++ classes for developing VRP solutions
 *      and specific solutions developed using these classes.
 *
 * Copyright 2014 Stephen Woodbridge <woodbri@imaptools.com>
 * Copyright 2014 Vicky Vergara <vicky_vergara@hotmail.com>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the MIT License. Please file LICENSE for details.
 *
 ********************************************************************VRP*/
#ifndef PG_TRASHCHECK_H
#define PG_TRASHCHECK_H


#include <string>
#include <sstream>

//#include "vrptools.h"
#include "prob_trash.h"

class Trash_check : public Prob_trash {

 private:
  std::vector<std::string> errorsFound;
  bool dataIsLatLon;

  template <typename T>
  std::string numbertoString ( T Number ) {
    std::stringstream ss;
    ss << Number;
    return ss.str();
  }

public:
  Trash_check(container_t *p_containers, unsigned int container_count,
              otherloc_t *p_otherlocs, unsigned int otherloc_count,
              ttime_t *p_ttimes, unsigned int ttime_count,
              vehicle_t *p_vehicles, unsigned int vehicle_count);


  bool CheckContainers(container_t *containers, int count);
  bool CheckOtherlocs(otherloc_t *otherlocs, int count);
  bool CheckTimes(ttime_t *ttimes, int count);
  bool CheckVehicles(vehicle_t *vehicles, int count);

  char **getErrorsForPg( int &count );
  bool isValid() const;
  std::string getErrorsString() const;
  void whatIsWrong() ;

};

#endif //  PG_TRASHCHECK_H
