/*
 * CddInterface: Gap interface to Cdd package
 */

#include "src/compiled.h"          /* GAP headers */
#include "setoper.h"
#include "cdd.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

/**********************************************************
*
*    Auxiliary functions to be used inside C 
* 
* ********************************************************/

char* A2String(int *n, int size)
{
        static char s1[dd_linelenmax],s[dd_linelenmax]= "";
        int i;
        strcat(s, " ");
        for( i=0;i<size;i++)
           {
           sprintf(s1, "%d", *(n+i) );
           strcat(s,s1);
           strcat(s, " ");
           } 
        return s;
}


dd_MatrixPtr ddG_CanonicalizeMatrix( dd_MatrixPtr M)
{
  dd_MatrixPtr N=M;
  dd_rowset impl_linset, redset;
  dd_rowindex newpos;
  dd_ErrorType err;
  dd_MatrixCanonicalize(&N, &impl_linset, &redset, &newpos, &err);
  
  return N;
}


dd_MatrixPtr ddG_PolyInput2Matrix (int k_rep, int k_numtype,int k_linearity, dd_rowrange k_rowrange, 
                         dd_colrange k_colrange,char k_linearity_array[50000],char k_matrix[50000],
                         int k_LPobject, char k_rowvec[50000])
                                  
{
char numbtype[50000], k_value[50000], k_matrixx[50000],k_linearity_arrayx[50000], k_rowvecx[50000];
dd_MatrixPtr M=NULL;
 dd_rowrange m_input,i;
 dd_colrange d_input,j;
 dd_RepresentationType rep;
 dd_boolean found=dd_FALSE, newformat=dd_FALSE, successful=dd_FALSE, linearity=dd_FALSE;
 dd_NumberType NT;
 dd_LPObjectiveType ob;
 mytype rational_value;
 static mytype value;
 char * pch;
 div_t z;
 int u;
 
 strcpy( k_matrixx, k_matrix);
 strcpy( k_linearity_arrayx, k_linearity_array );
 strcpy( k_rowvecx, k_rowvec );
 
// // creating the matrix with these two dimesnions
   M=dd_CreateMatrix(k_rowrange, k_colrange);
//   
// 
 // controling if the given representation is H or V.
   if( k_rep == 2 ) {
       rep=dd_Generator; newformat=dd_TRUE;
     } else if (k_rep == 1)
     {
       rep=dd_Inequality; newformat=dd_TRUE;
     } else 
     {
       rep=dd_Unspecified; newformat=dd_TRUE;
     }
 
   M->representation=rep;
//   
// controling the numbertype in the matrix
// this may seem silly, but it should be so for compatiblity with cdd.
   if (k_numtype==3) {  
   strcpy(numbtype, "integer");
   } else if (k_numtype==2) { 
     strcpy(numbtype, "rational");
   } else if (k_numtype==1) {
     strcpy(numbtype, "real");}
     else { strcpy(numbtype, "unspecified");}
 
   NT=dd_GetNumberType(numbtype);
//   
   M->numbtype=NT;
//   
//  controling the linearity of the given polygon.
   if (k_linearity==1) { 
     linearity= dd_TRUE;
     dd_SetLinearity(M,k_linearity_arrayx);
   }
//  
// // filling the matrix with elements scanned from the string k_matrix
    u=0;
    pch = strtok (k_matrixx," ,.{}][");
    while(pch != NULL) {
      
           strcpy( k_value, pch);
           dd_init( rational_value );
           dd_sread_rational_value (k_value, rational_value);
           dd_set(value,rational_value);
           dd_clear(rational_value);
           z= div(u, k_colrange );
           dd_set(M->matrix[z.quot][z.rem],value);
           u= u+1;
           pch = strtok (NULL, " ,.{}][");
       }
  successful=dd_TRUE;
  
  if (k_LPobject==0 ) { M->objective=dd_LPnone; } 
  else if ( k_LPobject == 1 ) { M->objective= dd_LPmax; } 
  else { M->objective = dd_LPmin; }
  
  
  if (k_LPobject==1 || k_LPobject==2 ) 
  {
  pch =strtok( k_rowvecx, " ,.{}][");
  for(u=0;u< M-> colsize; u++)
  {
    strcpy( k_value, pch );
    dd_init( rational_value );
    dd_sread_rational_value(k_value, rational_value );
    dd_set( value, rational_value );
    dd_clear( rational_value );
    dd_set( M->rowvec[u], value );
    pch= strtok( NULL, " ,.{}][" );
  }  
  }

  
  return M;
}





dd_LPSolutionPtr ddG_LPSolutionPtr( dd_MatrixPtr M )
{
  static dd_ErrorType err=dd_NoError;
  static dd_LPPtr lp;
  static dd_LPSolverType solver=dd_DualSimplex;
  static dd_LPSolutionPtr lps;
  
  lp=dd_Matrix2LP(M, &err);
  
  dd_LPSolve(lp, solver, &err);
  
  lps= dd_CopyLPSolution( lp );
  
  return  lps;
  
}


dd_rowrange ddG_RowSize( dd_MatrixPtr M)
{
  return M-> rowsize;
}

dd_colrange ddG_ColSize( dd_MatrixPtr M)
{
  return M-> colsize;
}

dd_rowset ddG_RowSet( dd_MatrixPtr M )
{
  return M->linset;
}



int ddG_LinearitySize( dd_MatrixPtr M )
{
  dd_rowrange r;
  dd_rowset s;
  int i,u;
  
  r= ddG_RowSize( M );
  s= ddG_RowSet( M );
  
  u=0;
  for(i=1;i<=r;i++)
    if (set_member(i, s) ) {
      u=u+1;
    }
    
    return u;
}
  

int * ddG_LinearityPtr(dd_MatrixPtr M )
{
  dd_rowrange r;
  dd_rowset s;
  int i,u;
  static int lin_array[50000];
  
  r= ddG_RowSize( M );
  s= ddG_RowSet( M );
  
  u=0;
  for(i=1;i<=r;i++)
    if (set_member(i, s) ) {
      lin_array[u]=i;
      u=u+1;
    }
    
    return lin_array;
}



long int * ddG_RowVecPtr( dd_MatrixPtr M )
{
  static long int RowVec_array[50000];
  static mpz_t u,v;
  int i,z1,z2;
  dd_Arow row_vector;
  mpz_init(u);mpz_init( v);
  
  row_vector=M->rowvec;
  
  for(i=0;i< M->colsize;i++)
  {
       mpq_get_num(u, *(row_vector+i) );
       mpq_get_den(v, *(row_vector+i) );
       z1=mpz_get_si( u );
       z2=mpz_get_si( v );
       RowVec_array[2*i]=z1;
       RowVec_array[2*i+1]=z2;
  }
  mpz_clear(u);mpz_clear(v);
  
  return RowVec_array;
} 







long int * ddG_AmatrixPtr( dd_MatrixPtr M )
{
  static long int Amatrix_array[50000];
  dd_rowrange r;
  dd_colrange s;
  dd_Amatrix Ma;
  int r1,s1,k,i,j,z1,z2;
  mpz_t u,v;
  
  mpz_init(u);mpz_init(v);
  
  r= ddG_RowSize( M );
  s= ddG_ColSize( M );
  Ma = M->matrix;
  
   for(i=0;i<r;i++)
     for(j=0;j<s;j++)
     {
       mpq_get_num(u,  *(*(Ma+i)+j)  );
       mpq_get_den(v,  *(*(Ma+i)+j)  );
         z1=mpz_get_si( u );
         z2=mpz_get_si( v );
        Amatrix_array[2*(i*s+j)]=z1;
        Amatrix_array[2*(i*s+j)+1]=z2;
     }
 return Amatrix_array;
  
}






int ddG_RepresentationType( dd_MatrixPtr M )
{ 
  return M->representation;
}

int ddG_NumberType( dd_MatrixPtr M )
{ 
  return M->numbtype; 
}

dd_Amatrix ddG_Matrix( dd_MatrixPtr M )
{ 
  return M-> matrix;
}



int ddG_IsOptimal( dd_MatrixPtr M )
{
  static dd_LPSolutionPtr lps;
  lps = ddG_LPSolutionPtr( M );
  
  if (lps->LPS==dd_Optimal) return 1; 
    else return 0;
}

static char * RATPLIST_STR( Obj list )
{
 static char s[dd_linelenmax];
 char s1[dd_linelenmax];
  int i,n,m, current;
  Obj current_num, current_den;
  
  strcpy( s, " " );
  
  if (!IS_PLIST( list ) ){
   ErrorMayQuit( "The argument is not a list", 0,0 );
   return NULL;
  }
  
  n= LEN_PLIST( list );
  m= n/2;
  
  for(i=0;i<m;i++)
  {
    current_num = ELM_PLIST( list, 2*i+1 );
    current_den = ELM_PLIST( list, 2*i+2 );
    
    if( !IS_INTOBJ( current_num ) || !IS_INTOBJ( current_den ) ) {
      ErrorMayQuit( "no integer entries", 0, 0 );
      return NULL;
    }
//     current= INT_INTOBJ( current_num );
    sprintf(s1, "%ld/%ld", INT_INTOBJ(current_num), INT_INTOBJ(current_den) );
    strcat(s,s1);
    strcat(s, " ");
  }
  
  return s;
}

static char * PLIST_STR( Obj list )
{
 static char s[dd_linelenmax];
 char s1[dd_linelenmax];
  int i,n, current;
  Obj current_obj;
  
  strcpy( s, " " );
  if (!IS_PLIST( list ) ){
   ErrorMayQuit( "not a plain list kemo", 0,0 );
   return NULL;
  }
  
  n= LEN_PLIST( list );
  
  for(i=0;i<n;i++)
  {
    current_obj= ELM_PLIST( list, i+1 );
    if( !IS_INTOBJ( current_obj ) ) {
      ErrorMayQuit( "no integer entries", 0, 0 );
      return NULL;
    }
    current= INT_INTOBJ( current_obj );
    sprintf(s1, "%d", current );
    strcat(s,s1);
    strcat(s, " ");
  }
  
  return s;
}


static Obj MatrixPtrToGapObj( dd_MatrixPtr M )
{
  Obj current, result;
  
  result= NEW_PLIST( T_PLIST_CYC, 7);
  SET_LEN_PLIST(result, 7);
  
  current= INTOBJ_INT( ddG_RepresentationType( M ) );
  SET_ELM_PLIST(result, 1, current  ); 
  CHANGED_BAG( result );
  
  current= INTOBJ_INT( ddG_NumberType( M ) );
  SET_ELM_PLIST(result, 2, current  );
  CHANGED_BAG( result );
  
  
  if ( ddG_LinearitySize( M ) == 0) SET_ELM_PLIST(result, 3, INTOBJ_INT( 0 )  );
  else SET_ELM_PLIST(result, 3, INTOBJ_INT( 1 )  );
  CHANGED_BAG( result );
  
  current= INTOBJ_INT( ddG_RowSize( M ) );
  SET_ELM_PLIST(result, 4, current  );
  CHANGED_BAG( result );
  
   
  
  current= INTOBJ_INT( ddG_ColSize( M ) );
  SET_ELM_PLIST(result, 5, current  );
  CHANGED_BAG( result );
    
   size_t i1, size1;
   int i, size;
   int * lin;
   
   lin =  ddG_LinearityPtr( M );
   size = ddG_LinearitySize( M );
   size1=size;
   
   current= NEW_PLIST((size1 > 0) ? T_PLIST_CYC : T_PLIST, size1);
   SET_LEN_PLIST( current, size1 );
 
   for(i=0;i<size;i++){
     i1=i;
     SET_ELM_PLIST( current, i1+1, INTOBJ_INT( *(lin +i ) ) );
     CHANGED_BAG( current );
   }
   
  SET_ELM_PLIST(result, 6, current  );
  CHANGED_BAG( result );
  
   long int *matrix;
  
   matrix= ddG_AmatrixPtr( M );
   size= 2*ddG_ColSize( M )*ddG_RowSize( M );
   size1= size;
  
   current= NEW_PLIST(T_PLIST_CYC, size1 );
   SET_LEN_PLIST( current, size1 );
 
   for(i=0;i<size;i++){
     i1=i;
     SET_ELM_PLIST( current, i1+1, INTOBJ_INT( *(matrix + i ) ) );
     CHANGED_BAG( current );
   }
   
  SET_ELM_PLIST(result, 7, current  );
  CHANGED_BAG( result );
   
  
  
  return result;
}

static dd_MatrixPtr GapInputToMatrixPtr( Obj input )
{
  
  int k_rep,k_numtype,k_linearity, k_rowrange, k_colrange, k_LPobject;
   char k_linearity_array[50000], k_matrix[50000],k_rowvec[50000];
  
  dd_set_global_constants();
  
   k_rep=       INT_INTOBJ( ELM_PLIST( input , 1 ) );
   k_numtype=   INT_INTOBJ( ELM_PLIST( input , 2 ) );
   k_linearity= INT_INTOBJ( ELM_PLIST( input , 3 ) );
   k_rowrange=  INT_INTOBJ( ELM_PLIST( input , 4 ) );
   k_colrange=  INT_INTOBJ( ELM_PLIST( input , 5 ) );
   k_LPobject=  INT_INTOBJ( ELM_PLIST( input , 8 ) );
  
  if (k_numtype==3) 
   strcpy( k_matrix,          PLIST_STR( ELM_PLIST( input , 7 ) ) );
  else 
   strcpy( k_matrix,          RATPLIST_STR( ELM_PLIST( input , 7 ) ) );
    
  strcpy( k_linearity_array, PLIST_STR( ELM_PLIST( input , 6 ) ) );
  strcpy( k_rowvec,          RATPLIST_STR( ELM_PLIST( input , 9 ) ) );

  return ddG_PolyInput2Matrix( k_rep , k_numtype, k_linearity, k_rowrange, k_colrange, k_linearity_array, k_matrix , k_LPobject, k_rowvec  );
}       


/**********************************************************
*
*    Converting functions
* 
* ********************************************************/


Obj MPZ_TO_GAPOBJ( mpz_t x )
{
  return INTOBJ_INT ( mpz_get_si( x ) );
}

Obj MPQ_TO_GAPOBJ( mpq_t x )
{
  mpz_t num, den;
  
  mpz_init(num);mpz_init( den );
  mpq_get_num( num, x );
  mpq_get_den( den, x );
  return QUO( MPZ_TO_GAPOBJ(num), MPZ_TO_GAPOBJ(den) );
}

Obj CINTLISTPtr_TOGAPPLIST(  int *list, int n1 )
{
  size_t i,n;
  Obj M;
   int r;
   n=n1;
  M = NEW_PLIST(T_PLIST_CYC, n);
   SET_LEN_PLIST(M, n);
  for ( i = 0; i < n; i++) {
        SET_ELM_PLIST(M, i+1, INTOBJ_INT( *(list +i) ) );
        CHANGED_BAG( M );
    }
    
  return M;
}

static  int* GAPPLIST_TOINTPtr( Obj list )
{
  static  int array[50000];
  int i, len;
  Obj current_obj;
  if (! IS_PLIST( list ) ) {
    ErrorMayQuit( "not a plain list saleh", 0,0 );
    return NULL;
  }
  
  len = LEN_PLIST( list );
  
  for( i=0;i<len;i++){
    current_obj= ELM_PLIST( list, i+1 );
    if ( !IS_INTOBJ( current_obj ) ) {
      ErrorMayQuit( "not integer entries",0,0 );
      return NULL; 
      
    }
  array[i]=INT_INTOBJ( current_obj );  
  }
  
  return array;
}


static Obj FaceWithDimAndInteriorPoint( dd_MatrixPtr M, dd_rowset R, dd_rowset S)
{
  dd_ErrorType err;
  dd_rowset LL, ImL, RR, SS, Lbasis;
  dd_rowrange i,iprev=0;
  dd_colrange j,dim;
  dd_LPSolutionPtr lps=NULL;
  dd_boolean success=dd_FALSE;
  Obj result, current2;
  
  result = NEW_PLIST(T_PLIST_CYC, 3);
  SET_LEN_PLIST( result, 3 );
  
  set_initialize(&LL, M->rowsize);
  set_initialize(&RR, M->rowsize);
  set_initialize(&SS, M->rowsize);
  set_copy(LL, M->linset); /* rememer the linset. */
  set_copy(RR, R); /* copy of R. */
  set_copy(SS, S); /* copy of S. */
  if (dd_ExistsRestrictedFace(M, R, S, &err)){
  
        set_uni(M->linset, M->linset, R);
        dd_FindRelativeInterior(M, &ImL, &Lbasis, &lps, &err);
        dim=M->colsize - set_card(Lbasis)-1;
        set_uni(M->linset, M->linset, ImL);
        
        SET_ELM_PLIST( result, 1, INTOBJ_INT( dim ) );
        
           size_t i1, size1;
           int i, size;
           int * lin;
           Obj current;
           
           lin =  ddG_LinearityPtr( M );
           size = ddG_LinearitySize( M );
           size1=size;
   
           current= NEW_PLIST((size1 > 0) ? T_PLIST_CYC : T_PLIST, size1);
           SET_LEN_PLIST( current, size1 );
 
           
  
           for(i=0;i<size;i++){
           i1=i;
           SET_ELM_PLIST( current, i1+1, INTOBJ_INT( *(lin +i ) ) );
           CHANGED_BAG( current );
           }
        
           
        SET_ELM_PLIST( result, 2, current );
        
           
           size_t j1,n;
           n = (lps->d) -2;
           
           current2  = NEW_PLIST( T_PLIST_CYC, n );
           SET_LEN_PLIST( current2 , n );  
            
          for (j=1; j <=n; j++) {
            j1=j;
            SET_ELM_PLIST( current2, j1 , MPQ_TO_GAPOBJ( lps->sol[ j ] ) );
            CHANGED_BAG( current2 );
            
          }        
           
        SET_ELM_PLIST( result, 3, current2 );
        CHANGED_BAG( result );
        
        dd_FreeLPSolution(lps);
        set_free(ImL);
        set_free(Lbasis);
        set_copy(M->linset, LL); /* restore the linset */
        set_free(LL);
        set_free(RR);
        set_free(SS);
  
  return result;
  } else {
      set_copy(M->linset, LL); /* restore the linset */
        set_free(LL);
        set_free(RR);
        set_free(SS);
  
  return INTOBJ_INT( 0 );
  }  
  
  
}
/**********************************************************
*
*    functions to be called from Gap 
* 
* ********************************************************/

static Obj CddInterface_DimAndInteriorPoint( Obj self, Obj main )
{
  dd_MatrixPtr M;
  dd_colrange mindim;
  dd_rowset R, S;
  dd_boolean rip=dd_TRUE;
  
//   mindim= INT_INTOBJ( min_dim );
  M= GapInputToMatrixPtr( main );
//   if (mindim>=M->colsize) mindim=M->colsize-1;
  set_initialize(&R, M->rowsize); 
  set_initialize(&S, M->rowsize);
  
  return FaceWithDimAndInteriorPoint( M,  R, S );
}


static Obj CddInterface_Canonicalize( Obj self,Obj main )
{
  static dd_MatrixPtr M,A;
  char d[50000];
  Obj linearity_array;
//   dd_ErrorType err=dd_NoError;
//  dd_PolyhedraPtr poly;
  dd_set_global_constants();
//   M =ddG_PolyInput2Matrix(2, 3, 1, 3, 3, " 2 1 3 " , " 1 1 1 0 1 1 0 2 2 ", 1, " 4 7 8 " );
  M= GapInputToMatrixPtr( main );
  
  
  
  dd_rowset impl_linset, redset;
  dd_rowindex newpos;
  dd_ErrorType err;
  dd_MatrixCanonicalize(&M, &impl_linset, &redset, &newpos, &err);
  dd_free_global_constants();
  return MatrixPtrToGapObj( M );
  
//   return INTOBJ_INT( 3 );
//     M= GapInputToMatrixPtr( main );
//   M= ddG_CanonicalizeMatrix( M );
//   linearity_array = ELM_PLIST( main, 4 );
//   strcpy( d, PLIST_STR(linearity_array) );
  
}

static Obj CddInterface_Compute_H_rep( Obj self, Obj main )
{
  static dd_MatrixPtr M, A;
  dd_PolyhedraPtr poly;
  dd_ErrorType err;
   dd_set_global_constants();
   err=dd_NoError;
   M= GapInputToMatrixPtr( main );
   poly=dd_DDMatrix2Poly(M, &err);
   A= A=dd_CopyInequalities(poly);
   dd_free_global_constants();
   return MatrixPtrToGapObj( A );
}

static Obj CddInterface_Compute_V_rep( Obj self, Obj main )
{
  static dd_MatrixPtr M, A;
  dd_PolyhedraPtr poly;
  dd_ErrorType err;
   dd_set_global_constants();
   err=dd_NoError;
   M= GapInputToMatrixPtr( main );
   poly=dd_DDMatrix2Poly(M, &err);
   A= A=dd_CopyGenerators(poly);
   dd_free_global_constants();
   return MatrixPtrToGapObj( A );
}


static Obj CddInterface_LpSolution( Obj self, Obj main )
{
  static dd_MatrixPtr M;
  mpz_t u,v;
  long int z1,z2;
  dd_ErrorType err;
  static dd_LPPtr lp;   
  dd_LPSolutionPtr lps;
  dd_LPSolverType solver;
  size_t n,i;
  dd_colrange j;
  dd_set_global_constants();
  solver=dd_DualSimplex;
  err=dd_NoError;
  Obj current, res;
  
  M= GapInputToMatrixPtr( main );
  lp=dd_Matrix2LP(M, &err);
  
  dd_LPSolve(lp, solver, &err);
  lps=dd_CopyLPSolution(lp);
  
  if ( lps->LPS==dd_Optimal ) {
   
   n= lps->d -1;
   res= NEW_PLIST(T_PLIST_CYC, 2 );
   SET_LEN_PLIST( res , 2 );  
   
   current  = NEW_PLIST( T_PLIST_CYC, n );
   SET_LEN_PLIST( current , n );  
   
   
   for (j=1; j<=n; j++) {
     i=j;
     SET_ELM_PLIST( current, i , MPQ_TO_GAPOBJ( lps->sol[j] ) );
     }
    
    SET_ELM_PLIST( res, 1, current );
    SET_ELM_PLIST( res, 2, MPQ_TO_GAPOBJ( lps-> optvalue ) );
    
    return res;
  }
    
  else 
    
    return INTOBJ_INT( 0 ); 
   
}
  
/******************************************************************/

typedef Obj (* GVarFunc)(/*arguments*/);

#define GVAR_FUNC_TABLE_ENTRY(srcfile, name, nparam, params) \
  {#name, nparam, \
   params, \
   (GVarFunc)name, \
   srcfile ":Func" #name }

// Table of functions to export
static StructGVarFunc GVarFuncs [] = {
  
//     GVAR_FUNC_TABLE_ENTRY("CddInterface.c", TestCommandWithParams, 2, "param, param2"),
//     GVAR_FUNC_TABLE_ENTRY("CddInterface.c", TestCommand_max, 2, "param1, param2"),
//     GVAR_FUNC_TABLE_ENTRY("CddInterface.c", testkamalove, 0, ""),
//     GVAR_FUNC_TABLE_ENTRY("CddInterface.c", take_it_and_give_it_back, 1, "list"),
    GVAR_FUNC_TABLE_ENTRY("CddInterface.c", CddInterface_Canonicalize, 1, "main"),
    GVAR_FUNC_TABLE_ENTRY("CddInterface.c", CddInterface_Compute_H_rep, 1, "main"),
    GVAR_FUNC_TABLE_ENTRY("CddInterface.c", CddInterface_Compute_V_rep, 1, "main"),
    GVAR_FUNC_TABLE_ENTRY("CddInterface.c", CddInterface_LpSolution, 1, "main"),
    GVAR_FUNC_TABLE_ENTRY("CddInterface.c", CddInterface_DimAndInteriorPoint, 1, "main"),
    
//     GVAR_FUNC_TABLE_ENTRY("CddInterface.c", take_poly_and_give_it_back, 1, "list"),
    
    { 0 } /* Finish with an empty entry */

};
 
/******************************************************************************
*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel( StructInitInfo *module )
{
    /* init filters and functions                                          */
    InitHdlrFuncsFromTable( GVarFuncs );

    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/
static Int InitLibrary( StructInitInfo *module )
{
    /* init filters and functions */
    InitGVarFuncsFromTable( GVarFuncs );

    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitInfopl()  . . . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
 /* type        = */ MODULE_DYNAMIC,
 /* name        = */ "CddInterface",
 /* revision_c  = */ 0,
 /* revision_h  = */ 0,
 /* version     = */ 0,
 /* crc         = */ 0,
 /* initKernel  = */ InitKernel,
 /* initLibrary = */ InitLibrary,
 /* checkInit   = */ 0,
 /* preSave     = */ 0,
 /* postSave    = */ 0,
 /* postRestore = */ 0
};

StructInitInfo *Init__Dynamic( void )
{
    return &module;
}
