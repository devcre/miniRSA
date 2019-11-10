/*
 * @file    rsa.c
 * @author  Kyoung chan Cho / 2015038622
 * @date    2019/11/10
 * @brief   mini RSA implementation code
 * @details 세부 설명
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rsa.h"
#include <math.h>

llint p, q, e, d, n;
llint GCD(llint a, llint b);


llint power(llint nu1, llint nu2){
    llint k;
    llint result = 1;

    for(k=0; k<nu2; k++){
        result *= nu1;
    }
    return result;
}

// equal to 'numb1 % numb2'
llint div_r(llint numb1, llint numb2){
    llint result = numb1;
    while(numb1 > numb2){
        result -= numb2;
    }
    return result;
}

llint div_q(llint numb1, llint numb2){
    llint result = numb1;
    llint count = 0;
    while(numb1 > numb2){
        result -= numb2;
        count += 1;
    }
    return count;
}

// // not active
// llint divide(llint nu1, llint nu2){
//     llint k = 0;
//     llint result = 0;

//     while(nu2 != 0){
//         while(power(2,k) < nu2) k++;
//         result += (nu1 >> k);
//         nu2 -= (llint)power(2,k);
//         k = 0;
//     }
//     return result;
// }

// distinguish whether the value is even or odd
// if num is odd, return TRUE, otherwise FALSE
bool isOdd(llint num){
    // printf("GCD(%lld,2): %lld\n", num, GCD(num,2));
    if(GCD(num,2) == 1){ 
        return TRUE;
    }
    else{
        return FALSE;
    }
}

// generate sudo-prime number
llint rndOddGen(){
    llint val;
    llint MAX = 999;
    llint MIN = 9;
    uint seedd = time(NULL);

    InitWELLRNG512a(&seedd);
    while(TRUE){
        val = (WELLRNG512a() * (MAX-MIN)) + MIN;
        if(isOdd(val)==TRUE){ // if val is odd
            break;
        }
    }
    return val;
}

/*
 * @brief     모듈러 덧셈 연산을 하는 함수.
 * @param     llint a     : 피연산자1.
 * @param     llint b     : 피연산자2.
 * @param     byte op    : +, - 연산자.
 * @param     llint n      : 모듈러 값.
 * @return    llint result : 피연산자의 덧셈에 대한 모듈러 연산 값. (a op b) mod n
 * @todo      모듈러 값과 오버플로우 상황을 고려하여 작성한다.
 */
llint ModAdd(llint a, llint b, byte op, llint n) {
    // should make overflow situation
    // 'a' and 'b' must be positive number
    llint result;
    while(a > n){
        a = a - n;
    }
    while(b > n){
        b = b - n;
    }

    if(op == '+'){
        result = a + b;
    }
    else{
        result = a - b;
        while(result < 0){
            result = result + n;
        }
    }

    while(result > n){
        result = result - n;
    }
    return result;
}

/*
 * @brief      모듈러 곱셈 연산을 하는 함수.
 * @param      llint x       : 피연산자1.
 * @param      llint y       : 피연산자2.
 * @param      llint n       : 모듈러 값.
 * @return     llint result  : 피연산자의 곱셈에 대한 모듈러 연산 값. (a x b) mod n
 * @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
 */
llint ModMul(llint x, llint y, llint n) {
    // should make overflow situation
    llint result;
    // (x * y)mod n = ((x mod n) * (y mod n))mod n
    while(x > n){
        x -= n;
    }
    while(y > n){
        y -= n;
    }

    result = x * y;
    
    while(result > n){
        result -= n;
    }
    return result;
}

/*
 * @brief      모듈러 거듭제곱 연산을 하는 함수.
 * @param      llint base   : 피연산자1.
 * @param      llint exp    : 피연산자2.
 * @param      llint n      : 모듈러 값.
 * @return     llint result : 피연산자의 연산에 대한 모듈러 연산 값. (base ^ exp) mod n
 * @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
               'square and multiply' 알고리즘을 사용하여 작성한다.
 */
llint ModPow(llint base, llint exp, llint n) {
    // should make overflow situation
    // should n is positive number
    llint result = 0;
    llint count = 0;

    // base^exp momd n = ((base mod n)^exp) mod n
    while(base > n){
        base = base - n;
    }

    // Recursive call
    if(exp > 1){
        if(isOdd(exp) != TRUE){
            return ModPow(base*base, exp>>1, n);
        }
        else{
            return ModMul(base, ModPow(base*base, (exp-1)>>1, n), n);
        }
    }
    else if(exp == 0){
        return 1;
    }
    else if(exp == 1){
        return base;
    }
    else{
        printf("Unappropriate value\n");
        return 1;
    }

    // if(n == 0){
    //     result = 1;
    // }
    // else if(n == 1){
    //     result = base;
    // }
    // else{
    //     while(exp > 1){
    //         if(isOdd(exp) == TRUE){ // if n is even -> "square and multiply" algo
    //             base = base * base;
    //             exp = exp >> 1; // divde 2
    //         }
    //         else{
    //             count += 1;
    //             base = base * base;
    //             exp = (exp - 1) >> 1; // divide 2
    //         }
    //         while(base > n){
    //             base = base - n;
    //         }
    //     }
    //     if(count == 0){
    //         count = 1;
    //     }
    //     result = ModMul(base, count, n);
    // }
    return result;
}

/*
 * @brief      입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
 * @param      llint testNum   : 임의 생성된 홀수.
 * @param      llint repeat    : 판단함수의 반복횟수.
 * @return     llint result    : 판단 결과에 따른 TRUE, FALSE 값.
 * @todo       Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
               이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다. 
 */
bool IsPrime(llint testNum, llint repeat) {
    bool result = TRUE;
    llint rnum;  // random integer 'a', 1 < a < testNum-1
    llint nn = testNum - 1;
    llint s = 0;
    llint dd = 0;

    // exception
    if(testNum == 2 || testNum ==3) return TRUE;
    if(testNum <= 1 || !(testNum & 1)) return result;

    llint MAX = nn;
    llint MIN = 1;

    // Find integers nn = 2^s*d
    dd = nn;
    while(TRUE){
        if(isOdd(dd) == TRUE){ // if dd is odd
            break;
        }
        s += 1;
        dd >>= 1; // divide with 2
    }

    for(int ktmp=0; ktmp<repeat; ktmp++){ // repeat 'ktmp' times
        while(TRUE){
            // Select random integer rnum ('a')
            rnum = (WELLRNG512a() * (MAX-MIN)) + MIN;
            if(isOdd(rnum)==TRUE){ // if val is odd
                break;
            }
        }
        for(llint i=0;i<s;i++){
            if((ModPow(rnum, power(2,i)*dd, testNum) != nn) && (ModPow(rnum, dd, testNum) != 1)){
                result = FALSE;
            }
        }
    }
    return result;
}

/*
 * @brief       모듈러 역 값을 계산하는 함수.
 * @param       llint a      : 피연산자1.
 * @param       llint m      : 모듈러 값.
 * @return      llint result : 피연산자의 모듈러 역수 값.
 * @todo        확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
 */
llint ModInv(llint a, llint m) {
    llint result;
    llint val;
    llint res;

    // llint mm = m;
    // llint tt, qq;
    // llint xx = 0;
    // llint yy = 1;
    
    // if(m == 1) return 1;
    // while(a > 1){
    //     qq = div_q(a,mm);
    //     tt = mm;
    //     mm = div_r(a,mm);
    //     a = tt;

    //     tt = xx;
    //     xx = yy - qq * xx;
    //     yy = tt;
    // }
    // if(yy < 0) yy += m;
    // result = yy;

    // BRUTE FORCE METHOD
    val = a;
    while(val > m){
        val -= m;
    }

    for(llint xc=1; xc<m; xc++){
        res = val*xc;
        while(res > m){
            res -= m;
        }
        if(res == 1){
            result = xc;
            break;
        }
    }
    return result;
}

/*
 * @brief     RSA 키를 생성하는 함수.
 * @param     llint *p   : 소수 p.
 * @param     llint *q   : 소수 q.
 * @param     llint *e   : 공개키 값.
 * @param     llint *d   : 개인키 값.
 * @param     llint *n   : 모듈러 n 값.
 * @return    void
 * @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
 */
void miniRSAKeygen(llint *p, llint *q, llint *e, llint *d, llint *n) {
    // Select two large primes at random
    while(TRUE){
        llint sudo_p;
        llint sudo_q;
        while(TRUE){
            sudo_p = rndOddGen();
            printf("random-number1 %lld selected.\n", sudo_p);
            if(IsPrime(sudo_p,10) == TRUE){
                printf("%lld may be Prime.\n\n",sudo_p);
                *p = sudo_p;
                break;
            }
            printf("%lld is not Prime.\n\n",sudo_p);
        }
        while(TRUE){
            sudo_q = rndOddGen();
            printf("random-number2 %lld selected.\n", sudo_q);
            if(IsPrime(sudo_q,10) == TRUE && sudo_q != *p){
                printf("%lld may be Prime.\n\n",sudo_q);
                *q = sudo_q;
                break;
            }
            printf("%lld is not Prime.\n\n",sudo_q);
        }
        break;
        // if(*p * *q > pow(2,53)){
        //     break;
        // }
    }

    *n = (*p)*(*q);

    llint phi_n;
    phi_n = (*p-1)*(*q-1);
    llint MAX = phi_n - 1;
    llint MIN = 2;

    // Select at random the excryption key e,, 1 < e < phi_n
    while(TRUE){
        while(TRUE){
            // Select random integer ('e')
            *e = (WELLRNG512a() * (MAX-MIN)) + MIN;
            if(isOdd(*e)==TRUE){ // if val is odd
                break;
            }
        }
        if((*e < phi_n) && (GCD(*e,phi_n) == 1)){
            break;
        }
    }
    // Solve equation e*d = 1 mod phi_n
    *d = ModInv(*e, phi_n);
}

/*
 * @brief     RSA 암복호화를 진행하는 함수.
 * @param     llint data   : 키 값.
 * @param     llint key    : 키 값.
 * @param     llint n      : 모듈러 n 값.
 * @return    llint result : 암복호화에 결과값
 * @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
 */
llint miniRSA(llint data, llint key, llint n) {
    llint result;
    result = ModPow(data,key,n);
    return result;
}

llint GCD(llint a, llint b) {
    llint prev_a;

    while(b != 0) {
        // printf("GCD(%lld, %lld)\n", a, b);
        prev_a = a;
        a = b;
        while(prev_a >= b) prev_a -= b;
        b = prev_a;
    }
    // printf("GCD(%lld, %lld)\n\n", a, b);
    return a;
}

int main(int argc, char* argv[]) {
    byte plain_text[4] = {0x12, 0x34, 0x56, 0x78};
    llint plain_data, encrpyted_data, decrpyted_data;
    uint seed = time(NULL);

    memcpy(&plain_data, plain_text, 4);

    // 난수 생성기 시드값 설정
    seed = time(NULL);
    InitWELLRNG512a(&seed);

    // RSA 키 생성
    miniRSAKeygen(&p, &q, &e, &d, &n);
    printf("0. Key generation is Success!\n ");
    printf("p : %lld\n q : %lld\n e : %lld\n d : %lld\n N : %lld\n\n", p, q, e, d, n);

    // RSA 암호화 테스트
    encrpyted_data = miniRSA(plain_data, e, n);
    printf("1. plain text : %lld\n", plain_data);    
    printf("2. encrypted plain text : %lld\n\n", encrpyted_data);

    // RSA 복호화 테스트
    decrpyted_data = miniRSA(encrpyted_data, d, n);
    printf("3. cipher text : %lld\n", encrpyted_data);
    printf("4. Decrypted plain text : %lld\n\n", decrpyted_data);

    // 결과 출력
    printf("RSA Decryption: %s\n", (decrpyted_data == plain_data) ? "SUCCESS!" : "FAILURE!");

    return 0;
}