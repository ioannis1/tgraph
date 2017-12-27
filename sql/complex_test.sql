CREATE EXTENSION complex;

SELECT '(1,2')::complex;

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

