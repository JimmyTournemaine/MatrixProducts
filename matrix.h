/**
 * \file matrix.h
 * \brief Matrix utilities.
 * \author Jimmy Tournemaine <jimmy.tournemaine@yahoo.fr>
 * \version 1.0
 * \date 31 jan. 2016
 *
 * Some utilities to use matrix easily.
 */

/**
 * \struct matrix
 * \brief Represent a matrix
 */
struct matrix {
    size_t rows;    /*!< The number of rows of the matrix */
    size_t cols;    /*!< The number of cols of the matrix */
    double ** data; /*!< Data values of the matrix */
};

/**
 * \brief Allocate memory and initialize a (m x n) matrix
 * \param m The number of rows of the matrix
 * \param n The number of cols of the matrix
 * \return The newly created matrix
 */
struct matrix * matrix_init(size_t rows, size_t cols);


/**
 * \brief Set a value of a matrix
 * \param pmat A pointer to the matrix you want to change a value
 * \param row The index of the row
 * \param col The index of the column
 */
void matrix_set(struct matrix * pmat, size_t row, size_t col, double value);

/**
 * \brief Get a value somewhere in the matrix
 * \param pmat A pointer to the matrix you want to get a value
 * \param row The index of the row
 * \param col The index of the column
 */
double matrix_get(struct matrix * pmat, size_t row, size_t col);

/**
 * \brief Print a matrix on stdout
 * \param pmat A pointer to the matrix to print
 */
void matrix_print(struct matrix * pmat);

/**
 * \brief Print a matrix in a file
 * \param pmat A pointer to the matrix to print
 * \param out A pointer to a file where the matrix will be print
 */
void matrix_fprint(struct matrix * pmat, FILE * out);

/**
 * \brief Destroy a matrix by freeing memory
 * \param pmat A pointer to the matrix to destroy
 */
void matrix_destroy(struct matrix * pmat);

