/* Test -mglibc and -muclibc not allowed together.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do link { target *-*-linux* } } */
/* { dg-options "-mglibc -muclibc" } */

/* { dg-message "-mglibc and -muclibc used together" "" { ! target or32-*-* } 0 } */
/* { dg-message "unrecognized command line option" "" { target or32-*-* } 0 } */
