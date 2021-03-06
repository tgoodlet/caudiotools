#include <math.h>    // for the square root function sqrt()     
#include <stdio.h>
#include <stdlib.h>
#include "wavereader.h"
#include "stats.h"



double **
xcorr(int maxdelay, buffer_t *sig1, buffer_t *sig2)
{
    //THE SHITTY O(n^2) way! (i.e. not using fft)
    int i,j; 
    double mx,my,sx,sy,sxy,denom;
    size_t n;       // length of longest vector

    double *y;
    double *x;
    double **R;

    /* hack to match vector lengths by zero padding */
    if(sig1->length >= sig2->length) {
        n = sig1->length;
        x = sig1->vector[0];
        y = sig2->vector[0];        //note y is the shifted vector
    }else{
        n = sig2->length;
        x = sig2->vector[0];
        y = sig1->vector[0];
    }

    
    /* allocate mem for output correlations and lags */
    R = malloc(2 * sizeof(double *));
    R[0] = calloc((size_t)(2*n - 1), sizeof(double));//contains lags
    R[1] = calloc((size_t)(2*n - 1), sizeof(double));//contains data

    /*calculate the mean of the two series x[], y[] */
    mx = 0;
    my = 0;

    for (i = 0; i < n; i++) {
        mx += x[i];
        my += y[i];
    }
    mx /= n;
    my /= n;

    /*Calc the denominator*/
    sx = 0;
    sy = 0;
    
    for (i = 0; i < n; i++) {
        sx += (x[i] - mx) * (x[i] - mx);
        sy += (y[i] - my) * (y[i] - my);
    }
    denom = sqrt(sx * sy);

    /* Calculate the correlation series */
    /* R(lag) = E[y(i+lag)*(x(i))] */
    
    if(maxdelay == 0)
        maxdelay = n - 1;    

    double lag = -maxdelay;

    for (int index = 0; index < 2*n-1; index++) {
        sxy=0;
        for(i = 0; i < n; i ++) {
            j = i + lag;
            if(j < 0 || j >= n)
                continue;
               //OR? sxy += (x[i] - mx) * (- my); 
            else
                sxy += (x[i] - mx) * (y[j] - my);
        } 
        R[0][index] = lag;
        R[1][index] = sxy / denom;
        lag ++;
    }
    return R;
}

    /* Calc the corr series - cirular boundary conditions 
    for (delay = -maxdelay; delay < maxdelay; delay++;)
        sxy=0;
        for(i = 0; i < n; i ++) {
            j = i + delay;
            while(j < 0)
                j += n;
            j %= n;
            sxy += (x[i] - mx) * (y[j] - my);
        }
        R = sxy / denom;*/

/*short a = 0xff00
(char[])a[0] == 0xff
((char [])a)[0] == 0xff
((char *)a)[0] == 0xff <-- most */
