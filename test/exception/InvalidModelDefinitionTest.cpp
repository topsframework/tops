/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

// External headers
#include "gmock/gmock.h"

// Tested header
#include "exception/InvalidModelDefinition.hpp"

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;

using tops::exception::InvalidModelDefinition;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

void throwAInvalidModelDefinitionException() {
  throw_exception(InvalidModelDefinition);
}

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(InvalidModelDefinition, ShouldThrowTheRigthException) {
  ASSERT_THROW(throwAInvalidModelDefinitionException(),
               InvalidModelDefinition);
}

/*----------------------------------------------------------------------------*/

TEST(InvalidModelDefinition, ShouldHaveTheRigthExceptionMessage) {
  try {
    throwAInvalidModelDefinitionException();
  } catch(InvalidModelDefinition& e) {
    ASSERT_STREQ(e.what(), "test/exception/InvalidModelDefinitionTest.cpp:39: "
                           "throwAInvalidModelDefinitionException: "
                           "Invalid model definition");
  }
}

/*----------------------------------------------------------------------------*/
