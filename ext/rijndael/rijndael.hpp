/**
 * rijndael-alg-fst.h
 *
 * @version 3.0 (December 2000)
 *
 * Optimised ANSI C code for the Rijndael cipher (now AES)
 *
 * @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * @author Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <valunpak/config.hpp>

namespace valunpak
{
    int rijndael_key_setup_enc(u32 rk[/*4*(Nr + 1)*/], const u8 cipherKey[], int keyBits);
    int rijndael_key_setup_dec(u32 rk[/*4*(Nr + 1)*/], const u8 cipherKey[], int keyBits);
    void rijndael_encrypt(const u32 rk[/*4*(Nr + 1)*/], int Nr, const u8 pt[16], u8 ct[16]);
    void rijndael_decrypt(const u32 rk[/*4*(Nr + 1)*/], int Nr, const u8 ct[16], u8 pt[16]);

    #ifdef INTERMEDIATE_VALUE_KAT
    void rijndael_encrypt_round(const u32 rk[/*4*(Nr + 1)*/], int Nr, u8 block[16], int rounds);
    void rijndael_decrypt_round(const u32 rk[/*4*(Nr + 1)*/], int Nr, u8 block[16], int rounds);
    #endif /* INTERMEDIATE_VALUE_KAT */
}
