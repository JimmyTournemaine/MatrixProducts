/**
 * \file generate.h
 * \brief A matrix generator
 * \author Jimmy Tournemaine <jimmy.tournemaine@yahoo.fr>
 * \version 1.0
 * \date 31 jan. 2016
 *
 * Generate a file containing random or edited matrix
 */

/**
 * \brief Pick a random integer grander than 1.
 * \param max The biggest integer you would get.
 * \return The random integer between 1 and \a max.
 */
int pickInt(int max);

/**
 * \brief Pick a random value
 * \return A random value
 */
double pickValue();
