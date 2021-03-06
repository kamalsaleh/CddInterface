# CddInterface, single 7
#
# DO NOT EDIT THIS FILE - EDIT EXAMPLES IN THE SOURCE INSTEAD!
#
# This file has been generated by AutoDoc. It contains examples extracted from
# the package documentation. Each example is preceded by a comment which gives
# the name of a GAPDoc XML file and a line range from which the example were
# taken. Note that the XML file in turn may have been generated by AutoDoc
# from some other input.
#
gap> START_TEST( "cddinterface07.tst");

# doc/_Chunks.xml:262-293
gap> P := Cdd_PolyhedronByGenerators( [ [ 1, 2, 5 ], [ 0, 1, 2 ] ] );
< Polyhedron given by its V-representation >
gap> Q := Cdd_PolyhedronByGenerators( [ [ 1, 4, 6 ], [ 1, 3, 7 ], [ 0, 3, 1 ] ] );
< Polyhedron given by its V-representation >
gap> S := P+Q;
< Polyhedron given by its H-representation >
gap> V := Cdd_V_Rep( S );
< Polyhedron given by its V-representation >
gap> Display( V );
V-representation 
begin
   4 X 3  rational

   0   3   1 
   1   6  11 
   1   5  12 
   0   1   2 
end
gap> Cdd_GeneratingVertices( P ); 
[ [ 2, 5 ] ]
gap> Cdd_GeneratingVertices( Q );
[ [ 3, 7 ], [ 4, 6 ] ]
gap> Cdd_GeneratingVertices( S );
[ [ 5, 12 ], [ 6, 11 ] ]
gap> Cdd_GeneratingRays( P );
[ [ 1, 2 ] ]
gap> Cdd_GeneratingRays( Q );
[ [ 3, 1 ] ]
gap> Cdd_GeneratingRays( S );
[ [ 1, 2 ], [ 3, 1 ] ]

#
gap> STOP_TEST("cddinterface07.tst", 1 );
