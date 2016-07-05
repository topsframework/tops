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
#include "exception/NotYetImplemented.hpp"

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;

using tops::exception::NotYetImplemented;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

void throwANotYetImplementedException() {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(NotYetImplemented, ShouldThrowTheRigthException) {
  ASSERT_THROW(throwANotYetImplementedException(), NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

TEST(NotYetImplemented, ShouldHaveTheRigthExceptionMessage) {
  try {
    throwANotYetImplementedException();
  } catch(NotYetImplemented& e) {
    ASSERT_STREQ(e.what(), "test/exception/NotYetImplementedTest.cpp:39: "
                           "throwANotYetImplementedException: "
                           "Method not implemented");
  }
}

/*----------------------------------------------------------------------------*/
