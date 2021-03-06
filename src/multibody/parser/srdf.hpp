//
// Copyright (c) 2016 CNRS
//
// This file is part of Pinocchio
// Pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// Pinocchio If not, see
// <http://www.gnu.org/licenses/>.

#ifndef __se3_parser_srdf_hpp__
#define __se3_parser_srdf_hpp__

#include "pinocchio/multibody/model.hpp"
#include <iostream>

// Read XML file with boost
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <boost/foreach.hpp>


namespace se3
{
  namespace srdf
  {
    
#ifdef WITH_HPP_FCL
    ///
    /// \brief Deactive all possible collision pairs mentioned in the SRDF file.
    ///        It throws if the SRDF file is incorrect.
    ///
    /// \param[in] filename The complete path to the SRDF file.
    /// \param[in] verbose Verbosity mode.
    ///
    inline void removeCollisionPairsFromSrdf(const GeometryModel & model_geom,
                                             GeometryData & data_geom,
                                             const std::string & filename,
                                             const bool verbose) throw (std::invalid_argument)
    {
      // Check extension
      const std::string extension = filename.substr(filename.find_last_of('.')+1);
      if (extension != "srdf")
      {
        const std::string exception_message (filename + " does not have the right extension.");
        throw std::invalid_argument(exception_message);
      }
      
      // Open file
      std::ifstream srdf_stream(filename.c_str());
      if (! srdf_stream.is_open())
      {
        const std::string exception_message (filename + " does not seem to be a valid file.");
        throw std::invalid_argument(exception_message);
      }
      
      // Read xml stream
      using boost::property_tree::ptree;
      ptree pt;
      read_xml(srdf_stream, pt);
      
      // Iterate over collision pairs
      const se3::Model & model = model_geom.model;
      BOOST_FOREACH(const ptree::value_type & v, pt.get_child("robot"))
      {
        if (v.first == "disable_collisions")
        {
          const std::string link1 = v.second.get<std::string>("<xmlattr>.link1");
          const std::string link2 = v.second.get<std::string>("<xmlattr>.link2");
          
          // Check first if the two bodies exist in model
          if (!model.existBodyName(link1) || !model.existBodyName(link2))
          {
            if (verbose)
              std::cout << "It seems that " << link1 << " or " << link2 << " do not exist in model. Skip." << std::endl;
            continue;
          }
          
          const Model::JointIndex id1 = model.getBodyId(link1);
          const Model::JointIndex id2 = model.getBodyId(link2);
          
          typedef GeometryModel::GeomIndexList GeomIndexList;
          const GeomIndexList & innerObject1 = model_geom.innerObjects.at(id1);
          const GeomIndexList & innerObject2 = model_geom.innerObjects.at(id2);
          
          for(GeomIndexList::const_iterator it1 = innerObject1.begin();
              it1 != innerObject1.end();
              ++it1)
          {
            for(GeomIndexList::const_iterator it2 = innerObject2.begin();
                it2 != innerObject2.end();
                ++it2)
            {
              data_geom.removeCollisionPair(CollisionPair(*it1, *it2));
            }
          }
          
        }
      } // BOOST_FOREACH
    }
    
#endif // ifdef WITH_HPP_FCL
    
  }
} // namespace se3

#endif // ifndef __se3_parser_srdf_hpp__
