#include "generate_key.h"

int generate_key_random(mpz_t key_N, mpz_t key_e, mpz_t key_d, int key_size) {
    if(key_size != 512 && key_size != 1024 && key_size != 2048) {
        return -1;
    }
    gmp_randstate_t grt;
    gmp_randinit_default(grt);         // 设置随机数生成算法为默认
    gmp_randseed_ui(grt, time(NULL));  // 设置随机化种子为当前时间

    mpz_t key_p, key_q;
    mpz_init(key_p);  // 初始化q和p大素数
    mpz_init(key_q);

    mpz_urandomb(key_p, grt, key_size / 2);  // 随机生成一个0-2^1024的一个数
    mpz_urandomb(key_q, grt, key_size / 2);

    mpz_nextprime(key_p, key_p);  //使用GMP自带的素数生成函数
    mpz_nextprime(key_q, key_q);
    // gmp_printf("key_p: %ZX\n\n", key_p);
    // gmp_printf("key_q: %ZX\n\n", key_q);

    mpz_mul(key_N, key_p, key_q);

    mpz_t key_f;
    mpz_init(key_f);
    mpz_sub_ui(key_p, key_p, 1);
    mpz_sub_ui(key_q, key_q, 1);
    mpz_mul(key_f, key_p, key_q);
    // gmp_printf("key_f: %ZX\n\n", key_f);

    mpz_set_ui(key_e, 65537);
    // gmp_printf("[public key] e: %ZX; N: %ZX\n\n", key_e, key_N);

    mpz_invert(key_d, key_e, key_f);
    // gmp_printf("[private key] d: %ZX; N: %ZX\n\n", key_d, key_N);

    mpz_clear(key_p);
    mpz_clear(key_q);
    mpz_clear(key_f);
    return 0;
}

int generate_key_with_p_and_q(mpz_t key_N, mpz_t key_e, mpz_t key_d, mpz_t key_p, mpz_t key_q) {
    mpz_mul(key_N, key_p, key_q);

    mpz_t key_f;
    mpz_init(key_f);
    mpz_sub_ui(key_p, key_p, 1);
    mpz_sub_ui(key_q, key_q, 1);
    mpz_mul(key_f, key_p, key_q);
    // gmp_printf("key_f: %ZX\n\n", key_f);

    mpz_set_ui(key_e, 65537);
    // gmp_printf("[public key] e: %ZX; N: %ZX\n\n", key_e, key_N);

    mpz_invert(key_d, key_e, key_f);
    // gmp_printf("[private key] d: %ZX; N: %ZX\n\n", key_d, key_N);

    mpz_clear(key_f);
    return 0;
}