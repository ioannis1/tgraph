CREATE EXTENSION complex;

SET complex.style = 'none';

SELECT '(1,2)'::complex  ;
SELECT '1,2j''::complex  ;
SELECT '1''::complex     ;
SELECT '2j''::complex    ;
SELECT '1-2j''::complex  ;

SELECT '(1,2)'::complex +   '(1,2)';
SELECT '(1,2)'::complex -   '(1,2)';
SELECT '(1,2)'::complex =   '(1,2)';
SELECT '(1,2)'::complex >   '(0,2)';
SELECT '(1,2)'::complex >=  '(8,2)';

SELECT mag( '(3,4)'::complex );
SELECT complex_mag_squared( '(3,4)'::complex );

SELECT  2 > '(3,4)'::complex;
SELECT 25 >= '(3,4)'::complex;

SELECT  ('(1,8)'::complex@);      -- complex conjugate
SELECT |'(3,4)'::complex ;        -- magnitude

SELECT complex_new(1,8);
SELECT complex_new(3,1);

SELECT complex_real('(-2,3)'::complex);
SELECT complex_img('(2,-3)'::complex);

SELECT complex_overlaps('(2,2)'::complex, '(1,1)');
SELECT complex_overlaps('(2,2)'::complex, '(3,2)');

SELECT complex_mult( '(2,7)'::complex, '(3,9)');
SELECT complex_mult( '(3,2)'::complex, '(1,7)');
SELECT complex_mult( '(1,1)'::complex, '(1,1)');

SELECT complex_polar( '(4,3)');
SELECT complex_xy('(5,36.8699)');
SELECT complex_xy(complex_polar( '(4,3)'));
SELECT complex_polar(complex_theta_add('(4,3)',20));


