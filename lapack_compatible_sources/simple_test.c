#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "blis.h"
#include "FLAME.h"
#include "NoFLA_HQRRP_WY_blk_var4.h"

#ifndef max
#define max( a, b ) ( (a) > (b) ? (a) : (b) )
#endif
#ifndef min
#define min( a, b ) ( (a) < (b) ? (a) : (b) )
#endif

#define PRINT_DATA


// ============================================================================
// Declaration of local prototypes.

static void matrix_generate( int m_A, int n_A, double * buff_A, int ldim_A );

static void print_double_matrix( char * name, int m_A, int n_A, 
                double * buff_A, int ldim_A );

static void print_double_vector( char * name, int n, double * vector );

static void print_int_vector( char * name, int n, int * vector );

static void init_pvt( int n, int * vector );

static void set_pvt_to_zero( int n_p, int * buff_p );



// ============================================================================
int main( int argc, char *argv[] ) {
  int     nb_alg, pp, m_A, n_A, mn_A, ldim_A, ldim_Q, info, lwork;
  struct timespec t_start, t_end;
  double  elapsed, gflops;
  double  * buff_A, * buff_tau, * buff_Q, * buff_wk_qp4, * buff_wk_orgqr;
  int     * buff_p;

  if( argc < 2 ) {
    fprintf( stderr, "Usage: %s <matrix_size>\n", argv[0] );
    return 1;
  }
  m_A = n_A = atoi( argv[1] );
  if( m_A <= 0 ) {
    fprintf( stderr, "Error: matrix_size must be a positive integer.\n" );
    return 1;
  }

  mn_A     = min( m_A, n_A );
  buff_A   = ( double * ) malloc( m_A * n_A * sizeof( double ) );
  ldim_A   = max( 1, m_A );

  buff_p   = ( int * ) malloc( n_A * sizeof( int ) );

  buff_tau = ( double * ) malloc( n_A * sizeof( double ) );

  buff_Q   = ( double * ) malloc( m_A * mn_A * sizeof( double ) );
  ldim_Q   = max( 1, m_A );

  // Generate matrix with random values in [-1, 1].
  srand( 42 );
  matrix_generate( m_A, n_A, buff_A, ldim_A );

  // Initialize pivot vector to zero (free pivoting).
  set_pvt_to_zero( n_A, buff_p );

#ifdef PRINT_DATA
  if( n_A <= 16 ) {
    print_double_matrix( "ai", m_A, n_A, buff_A, ldim_A );
    print_int_vector( "pi", n_A, buff_p );
  }
#endif

  // Create workspace.
  lwork       = max( 1, 128 * n_A );
  buff_wk_qp4 = ( double * ) malloc( lwork * sizeof( double ) );

  // Factorize matrix.
  clock_gettime( CLOCK_MONOTONIC, &t_start );
  dgeqp4( & m_A, & n_A, buff_A, & ldim_A, buff_p, buff_tau,
          buff_wk_qp4, & lwork, & info );
  clock_gettime( CLOCK_MONOTONIC, &t_end );

  elapsed = ( t_end.tv_sec  - t_start.tv_sec  )
          + ( t_end.tv_nsec - t_start.tv_nsec ) * 1.0e-9;
  gflops  = ( 4.0 / 3.0 * (double) n_A * (double) n_A * (double) n_A ) / ( elapsed * 1.0e9 );

  printf( "%% n = %d\n", n_A );
  printf( "%% Time  : %.6f sec\n", elapsed );
  printf( "%% GFLOPS: %.4f\n", gflops );
  printf( "%% Info  : %d\n", info );

  // Remove workspace.
  free( buff_wk_qp4 );

  // Build matrix Q.
  lwork     = max( 1, 128 * n_A );
  buff_wk_orgqr = ( double * ) malloc( lwork * sizeof( double ) );
  dlacpy_( "All", & m_A, & mn_A, buff_A, & ldim_A, buff_Q, & ldim_Q );
  dorgqr_( & m_A, & mn_A, & mn_A, buff_Q, & ldim_Q, buff_tau,
           buff_wk_orgqr, & lwork, & info );
  if( info != 0 ) {
    fprintf( stderr, "Error in dorgqr: Info: %d\n", info );
  }
  free( buff_wk_orgqr );

#ifdef PRINT_DATA
  if( n_A <= 16 ) {
    print_double_matrix( "af", m_A, n_A, buff_A, ldim_A );
    print_int_vector( "pf", n_A, buff_p );
    print_double_vector( "tauf", n_A, buff_tau );
    print_double_matrix( "qf", m_A, mn_A, buff_Q, ldim_Q );
  }
#endif

  // Free matrices and vectors.
  free( buff_A );
  free( buff_p );
  free( buff_tau );
  free( buff_Q );

  printf( "%% End of Program\n" );

  return 0;
}

// ============================================================================
static void matrix_generate( int m_A, int n_A, double * buff_A, int ldim_A ) {
  int  i, j;

  for ( j = 0; j < n_A; j++ ) {
    for ( i = 0; i < m_A; i++ ) {
      buff_A[ i + j * ldim_A ] = 2.0 * ( ( double ) rand() / RAND_MAX ) - 1.0;
    }
  }
}

// ============================================================================
static void print_double_matrix( char * name, int m_A, int n_A, 
                double * buff_A, int ldim_A ) {
  int  i, j;

  printf( "%s = [\n", name );
  for( i = 0; i < m_A; i++ ) {
    for( j = 0; j < n_A; j++ ) {
      printf( "%le ", buff_A[ i + j * ldim_A ] );
    }
    printf( "\n" );
  }
  printf( "];\n" );
}

// ============================================================================
static void print_double_vector( char * name, int n_v, double * buff_v ) {
  int  i, j;

  printf( "%s = [\n", name );
  for( i = 0; i < n_v; i++ ) {
    printf( "%le\n", buff_v[ i ] );
  }
  printf( "\n" );
  printf( "];\n" );
}

// ============================================================================
static void print_int_vector( char * name, int n_v, int * buff_v ) {
  int  i, j;

  printf( "%s = [\n", name );
  for( i = 0; i < n_v; i++ ) {
    printf( "%d\n", buff_v[ i ] );
  }
  printf( "];\n" );
}

// ============================================================================
static void init_pvt( int n_p, int * buff_p ) {
  int  i;

  for( i = 0; i < n_p; i++ ) {
    buff_p[ i ] = ( i + 1 );
  }
}

// ============================================================================
static void set_pvt_to_zero( int n_p, int * buff_p ) {
  int  i;

  for( i = 0; i < n_p; i++ ) {
    buff_p[ i ] = 0;
  }
}

