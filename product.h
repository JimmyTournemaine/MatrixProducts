/**
 * \file product.h
 * \brief Calculate matrix products from a structured file
 * \author Jimmy Tournemaine <jimmy.tournemaine@yahoo.fr>
 * \version 1.0
 * \date 03 feb. 2016
 *
 * Some utilities to use matrix easily.
 */

/**
 * \enum State
 * \brief Describe the program state
 */
typedef enum
{
    STATE_WAIT, /*!< Threads are waiting to start */
    STATE_CALC, /*!< Threads can calculate */
    STATE_PRINT /*!< Threads have finished their task, the main program can print the result */
} State;

/**
 * \typedef Calculation
 * \brief The main structure (global) use by all threads to calculate
 */
typedef struct
{
    State state;            /*!< The state of the program */
    pthread_cond_t cond;    /*!< The condition on the semaphore */
    pthread_mutex_t mutex;  /*!< The semaphore */
    size_t nbMult;          /*!< The number of product that the program have to calculate */
    size_t size;            /*!< The size of the biggest matrix */
    size_t * pendingCoefs;  /*!< Coefficients which are pending to calculate */
    struct matrix * m1;     /*!< The first matrix of the product */
    struct matrix * m2 ;    /*!< The second matrix of the product */
    struct matrix * result; /*!< The result of the product */
} Calculation;

/**
 * \brief Return the maximum number of threads need.
 * \param mat1 Le tableau des M1
 * \param mat2 Le tableau des M2
 * \return Le nombre de threads
 */
size_t nbThread(struct matrix ** mat1, struct matrix **mat2);

/**
 * \brief Read a double from a string and forward the cursor
 * \param Un pointeur sur la chaîne à parser
 * \return Le nombre lu
 */
double scanDouble(char ** str);

/**
 * \brief Initialize pending coefficients
 * \param prod Un pointeur sur la structure de calcul
 */
void initPendingCoefs(Calculation * prod);

/**
 * \brief Count how many coefficients are pending to calculate
 * \param prod Un pointeur sur la structure de calcul
 * \return Le nombre de coefficients en attente de calcul
 */
int nbPendingCoefs(Calculation * prod);


