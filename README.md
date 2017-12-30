-- COPYRIGHT (c) Ioannis Tambouras 2011-2013

You may set the GUC variable complex.style='polar' to set output to polar coordinates, or to 'j' to display in Cartisian
coordinates without parenthesis, such as  1,2j ; similarly, set it to 'i' to display in Cartisian with 'i' ; 

# constructors

SELECT '1+2j'::complex                         => (1,2)

SELECT '2j'::complex                           => (0,2)

SELECT '2i'::complex                           => (0,2)

SELECT '1'::complex                            => (1,0)

SELECT '(1,2)'::complex                        => (1,2)

SELECT '1,2j'::complex                         => (1,2)

SELECT '2.236<63.435>'::complex                => (1,2)

SELECT complex_new(1, 2)                       => (1,2)

SELECT complex_new_polar( 2.236,63.435 )       => (1,2)

# Misc

SELECT complex_real( '(3,5)' )                   => 3 

SELECT complex_img(  '(3,5)' )                   => 5 

SELECT complex_dot('3,4j','3,-4j');             => 25

SELECT complex_angle_add('4,4j',20);            => 5.657<65>


# Magnitude, magnitude squared

SELECT |  '(3,4)'::complex                      => 5

SELECT complex_mag('(3,4)'::complex)            => 5

SELECT |^ '(3,4)'::complex                      => 25

SELECT complex_mag_squared('(3,4)')             => 25

# conjugate

SELECT complex_conjugate('(2,4)')             => (2,-4)

SELECT '(2,4)'::complex @                     => (2,-4)

# Arithmetic

SELECT '(2,4)' + '(1,2)'::complex            => (3,6)

SELECT '(2,4)' - '(2,2)'::complex            => (0,2)

SELECT '(2,4)' *  (2,2)'::complex            => (-4,12)

# Conversion from cartesian to polar 

SELECT complex_polar( '(7,-5)' )             => (8.60233,-35.5377)  -- meaning r=8.6 angle=-35

SELECT complex_angle( '(7,-5)' )             => -35.5377

# Conversion from polar to cartesian 

-- r= 8, angle=35

SELECT complex_xy( '(8,35)' )               => (6.55322,4.58861)   


# Relational operators

SELECT '(1,1)' <   '(4,4)'::complex        => true

SELECT '(1,1)' <=  '(4,4)'::complex        => true

SELECT '(4,4)' >=  '(1,1)'::complex        => true

SELECT '(4,4)' >=  '(4,4)'::complex        => true

SELECT '(4,4)' >   '(4,1)'::complex        => true

SELECT '(4,4)' =   '(4,4)'::complex        => true


SELECT complex_abs_cmp( '(4,4)' , '(4,4)' )    => 0

SELECT complex_abs_cmp( '(4,4)' , '(1,1)' )    => 1

SELECT complex_abs_cmp( '(1,1)' , '(4,4)' )    => -1


