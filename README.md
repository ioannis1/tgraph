# constructors

# SELECT complex_new( 1,2 )                    => (1,2)
# SELECT complex_new_polar( 8,30 )               => (6.92821,4)

# Misc
SELECT complex_real( '(3,5)' )                   => 3 
SELECT complex_img(  '(3,5)' )                   => 5 

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
SELECT complex_theta( '(7,-5)' )             => -35.5377

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

