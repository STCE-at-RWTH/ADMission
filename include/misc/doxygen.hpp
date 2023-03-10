#ifndef ADM_INC_DOXYGEN_HPP_
#define ADM_INC_DOXYGEN_HPP_

// ############################# HEADER CONTENTS ############################ //

// ----------------------------- Namespace docs ----------------------------- //

/******************************************************************************
 * @namespace admission
 * @brief Main namespace of thesis project ad-mission
 *
 * Main namespace of the ADMission project.
 * Contains a set of functions operating on boost graph types
 * and solver classes for finding optimal face-eliminations
 * sequences on DAGs.
 ******************************************************************************/

// ----------------------------- Doxygen groups ----------------------------- //

/******************************************************************************
 * @defgroup Dag DAG containers & properties
 *
 * Definitions of containers and their properties to store DAGs, face
 * DAGs and meta DAGs.
 ******************************************************************************/

/******************************************************************************
 * @defgroup DagCreate Creating and transforming DAGs
 ******************************************************************************/

/******************************************************************************
 * @defgroup DagInfo DAG Information
 *
 * Information about the structure of DAGs and FaceDAGs.
 ******************************************************************************/

/******************************************************************************
 * @defgroup DagIo DAG I/O
 *
 * Reading and writing DAGs and FaceDAGs from and to files.
 ******************************************************************************/

/******************************************************************************
 * @defgroup Algorithms Graph algorithms
 *
 * Elimination, Merge and Preaccumulation algorithms and helper functions.
 ******************************************************************************/

/******************************************************************************
 * @defgroup Exceptions Exceptions
 ******************************************************************************/

/******************************************************************************
 * @defgroup Misc Miscellaneous
 *
 * Miscellaneous overloads and additions to external libs.
 ******************************************************************************/

/******************************************************************************
 * @defgroup OpSequence Operation sequences
 *
 * Storing and applying Accumulations and Eliminations in sequences.
 ******************************************************************************/

/******************************************************************************
 * @defgroup Optimizers Optimizers
 *
 * Optimizers with algorithms for solving Jacobian accumulation on a FaceDAG.
 ******************************************************************************/

/******************************************************************************
 * @defgroup TestUtils Test utilities
 *
 * Utilities for unit tests.
 ******************************************************************************/

// -------------------------- Doxygen group macros -------------------------- //

//! Start of a doxygen module group.
#define DOXYGEN_MODULE_BEGIN(NAME) /** @addtogroup NAME */ /** @{ */

//! End of a doxygen module group.
#define DOXYGEN_MODULE_END(NAME) /** @} */

//! Start of a doxygen member group.
#define DOXYGEN_GROUP_BEGIN(NAME, DESC) /** DESC @name NAME */ /** @{ */

//! End of a doxygen member group.
#define DOXYGEN_GROUP_END() /** @} */

// ################################## EOF ################################### //

#endif  // ADM_INC_DOXYGEN_HPP_
