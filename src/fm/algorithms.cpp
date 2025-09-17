/*
 * SortaSound - Advanced FM Synthesizer
 * Copyright (C) 2024  Paige Thompson <paige@paige.bio>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "fm/fm.hpp"
#include <cmath>

namespace toybasic {

/**
 * @brief Process Algorithm 0: Pure serial chain 6→5→4→3→2→1
 * 
 * This algorithm implements a pure serial chain where operator 6 modulates
 * operator 5, which modulates operator 4, and so on down to operator 1.
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm0(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    return generateOperatorOutput(ops[0], mod2);
}

/**
 * @brief Process Algorithm 1: Parallel modulators (5+6)→4→3→2→1
 * 
 * This algorithm has operators 5 and 6 as parallel modulators that both
 * modulate operator 4, which then modulates operator 3, and so on.
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm1(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod5 + mod6);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    return generateOperatorOutput(ops[0], mod2);
}

/**
 * @brief Process Algorithm 2: Split path 6→5→4→3→2, 6→1
 * 
 * This algorithm creates two separate paths: one where operator 6 modulates
 * a chain 5→4→3→2, and another where operator 6 directly modulates operator 1.
 * Both operators 1 and 2 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 2
 */
double FMSynthesizer::processAlgorithm2(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    double mod1 = generateOperatorOutput(ops[0], mod6);
    return mod2 + mod1;
}

/**
 * @brief Process Algorithm 3: Split path 6→5→4→3, 6→2→1
 * 
 * This algorithm creates two separate paths: one where operator 6 modulates
 * a chain 5→4→3, and another where operator 6 modulates 2→1.
 * Both operators 1 and 3 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 3
 */
double FMSynthesizer::processAlgorithm3(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    
    double mod2 = generateOperatorOutput(ops[1], mod6);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod3 + mod1;
}

/**
 * @brief Process Algorithm 4: Split path 6→5→4, 6→3→2→1
 * 
 * This algorithm creates two separate paths: one where operator 6 modulates
 * a chain 5→4, and another where operator 6 modulates 3→2→1.
 * Both operators 1 and 4 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 4
 */
double FMSynthesizer::processAlgorithm4(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    
    double mod3 = generateOperatorOutput(ops[2], mod6);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod4 + mod1;
}

/**
 * @brief Process Algorithm 5: Split path 6→5, 6→4→3→2→1
 * 
 * This algorithm creates two separate paths: one where operator 6 modulates
 * operator 5, and another where operator 6 modulates 4→3→2→1.
 * Both operators 1 and 5 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 5
 */
double FMSynthesizer::processAlgorithm5(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    
    double mod4 = generateOperatorOutput(ops[3], mod6);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod5 + mod1;
}

/**
 * @brief Process Algorithm 6: Triple split 6→5→4, 6→3, 6→2→1
 * 
 * This algorithm creates three separate paths from operator 6:
 * - 6→5→4 (operators 4, 3, 1 are carriers)
 * - 6→3 (operator 3 is a carrier)
 * - 6→2→1 (operator 1 is a carrier)
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1, 3, and 4
 */
double FMSynthesizer::processAlgorithm6(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    
    double mod3 = generateOperatorOutput(ops[2], mod6);
    
    double mod2 = generateOperatorOutput(ops[1], mod6);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod4 + mod3 + mod1;
}

/**
 * @brief Process Algorithm 7: Quad split 6→5, 6→4, 6→3, 6→2→1
 * 
 * This algorithm creates four separate paths from operator 6:
 * - 6→5 (operator 5 is a carrier)
 * - 6→4 (operator 4 is a carrier)
 * - 6→3 (operator 3 is a carrier)
 * - 6→2→1 (operator 1 is a carrier)
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1, 3, 4, and 5
 */
double FMSynthesizer::processAlgorithm7(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod6);
    double mod3 = generateOperatorOutput(ops[2], mod6);
    
    double mod2 = generateOperatorOutput(ops[1], mod6);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod5 + mod4 + mod3 + mod1;
}

/**
 * @brief Process Algorithm 8: Mixed routing 6→5→3, 4→1, 2→1
 * 
 * This algorithm has mixed routing where:
 * - Operator 6 modulates 5→3
 * - Operator 4 directly modulates operator 1
 * - Operator 2 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm8(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod3 = generateOperatorOutput(ops[2], mod5);
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod4 + mod2);
}

/**
 * @brief Process Algorithm 9: Mixed routing (5+6)→4→1, 3→1
 * 
 * This algorithm has mixed routing where:
 * - Operators 5 and 6 in parallel modulate operator 4, which modulates operator 1
 * - Operator 3 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm9(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod5 + mod6);
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    
    return generateOperatorOutput(ops[0], mod4 + mod3);
}

/**
 * @brief Process Algorithm 10: Triple parallel (4+5+6)→3→1, 2→1
 * 
 * This algorithm has mixed routing where:
 * - Operators 4, 5, and 6 in parallel modulate operator 3, which modulates operator 1
 * - Operator 2 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm10(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod5 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2);
}

/**
 * @brief Process Algorithm 11: Triple parallel (4+5+6)→3→1, 2→1 (duplicate of 10)
 * 
 * This algorithm is identical to Algorithm 10 with mixed routing where:
 * - Operators 4, 5, and 6 in parallel modulate operator 3, which modulates operator 1
 * - Operator 2 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm11(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod5 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2);
}

/**
 * @brief Process Algorithm 12: Triple parallel (5+6)→2→1, 4→1, 3→1
 * 
 * This algorithm has mixed routing where:
 * - Operators 5 and 6 in parallel modulate operator 2, which modulates operator 1
 * - Operator 4 directly modulates operator 1
 * - Operator 3 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm12(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod5 + mod6);
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    
    return generateOperatorOutput(ops[0], mod2 + mod4 + mod3);
}

/**
 * @brief Process Algorithm 13: Triple parallel (5+6)→2→1, 4→1, 3→1 (duplicate of 12)
 * 
 * This algorithm is identical to Algorithm 12 with mixed routing where:
 * - Operators 5 and 6 in parallel modulate operator 2, which modulates operator 1
 * - Operator 4 directly modulates operator 1
 * - Operator 3 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm13(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod5 + mod6);
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    
    return generateOperatorOutput(ops[0], mod2 + mod4 + mod3);
}

/**
 * @brief Process Algorithm 14: Triple parallel (4+6)→3→1, 2→1, 5→1
 * 
 * This algorithm has mixed routing where:
 * - Operators 4 and 6 in parallel modulate operator 3, which modulates operator 1
 * - Operator 2 directly modulates operator 1
 * - Operator 5 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm14(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2 + mod5);
}

/**
 * @brief Process Algorithm 15: Triple parallel (4+6)→3→1, 2→1, 5→1 (duplicate of 14)
 * 
 * This algorithm is identical to Algorithm 14 with mixed routing where:
 * - Operators 4 and 6 in parallel modulate operator 3, which modulates operator 1
 * - Operator 2 directly modulates operator 1
 * - Operator 5 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm15(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2 + mod5);
}

/**
 * @brief Process Algorithm 16: Triple parallel (5+6)→4→1, 3→1, 2→1
 * 
 * This algorithm has mixed routing where:
 * - Operators 5 and 6 in parallel modulate operator 4, which modulates operator 1
 * - Operator 3 directly modulates operator 1
 * - Operator 2 directly modulates operator 1
 * Only operator 1 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 1
 */
double FMSynthesizer::processAlgorithm16(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod5 + mod6);
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod4 + mod3 + mod2);
}

/**
 * @brief Process Algorithm 17: Mixed routing (2+6)→4→5, 1
 * 
 * This algorithm has mixed routing where:
 * - Operators 2 and 6 in parallel modulate operator 4, which modulates operator 5
 * - Operator 1 acts as a direct carrier
 * Both operators 1 and 5 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 5
 */
double FMSynthesizer::processAlgorithm17(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod2 + mod6);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[4], mod4);
}

/**
 * @brief Process Algorithm 18: Triple parallel (3+5+6)→2→4, 1
 * 
 * This algorithm has mixed routing where:
 * - Operators 3, 5, and 6 in parallel modulate operator 2, which modulates operator 4
 * - Operator 1 acts as a direct carrier
 * Both operators 1 and 4 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 4
 */
double FMSynthesizer::processAlgorithm18(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod3 + mod5 + mod6);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod2);
}

/**
 * @brief Process Algorithm 19: Mixed routing (3+6)→2→4, 5→4, 1
 * 
 * This algorithm has mixed routing where:
 * - Operators 3 and 6 in parallel modulate operator 2, which modulates operator 4
 * - Operator 5 directly modulates operator 4
 * - Operator 1 acts as a direct carrier
 * Both operators 1 and 4 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 4
 */
double FMSynthesizer::processAlgorithm19(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod3 + mod6);
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod2 + mod5);
}

/**
 * @brief Process Algorithm 20: Mixed routing (2+6)→3→4, 5→4, 1
 * 
 * This algorithm has mixed routing where:
 * - Operators 2 and 6 in parallel modulate operator 3, which modulates operator 4
 * - Operator 5 directly modulates operator 4
 * - Operator 1 acts as a direct carrier
 * Both operators 1 and 4 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 4
 */
double FMSynthesizer::processAlgorithm20(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod2 + mod6);
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod3 + mod5);
}

/**
 * @brief Process Algorithm 21: Mixed routing (2+6)→3→4, 5→4, 1 (duplicate of 20)
 * 
 * This algorithm is identical to Algorithm 20 with mixed routing where:
 * - Operators 2 and 6 in parallel modulate operator 3, which modulates operator 4
 * - Operator 5 directly modulates operator 4
 * - Operator 1 acts as a direct carrier
 * Both operators 1 and 4 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 1 and 4
 */
double FMSynthesizer::processAlgorithm21(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod2 + mod6);
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod3 + mod5);
}

/**
 * @brief Process Algorithm 22: All parallel (1+2+3+4+5)→6
 * 
 * This algorithm has all operators 1-5 in parallel modulating operator 6.
 * Only operator 6 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 6
 */
double FMSynthesizer::processAlgorithm22(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5);
}

/**
 * @brief Process Algorithm 23: All parallel (1+2+3+4+5)→6 (duplicate of 22)
 * 
 * This algorithm is identical to Algorithm 22 with all operators 1-5 in parallel
 * modulating operator 6. Only operator 6 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 6
 */
double FMSynthesizer::processAlgorithm23(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5);
}

/**
 * @brief Process Algorithm 24: Mixed routing (3+5+6)→4, 1→2
 * 
 * This algorithm has mixed routing where:
 * - Operators 3, 5, and 6 in parallel modulate operator 4
 * - Operator 1 modulates operator 2
 * Both operators 2 and 4 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 2 and 4
 */
double FMSynthesizer::processAlgorithm24(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    double out2 = generateOperatorOutput(ops[1], mod1);
    double out4 = generateOperatorOutput(ops[3], mod3 + mod5 + mod6);
    
    return out2 + out4;
}

/**
 * @brief Process Algorithm 25: Mixed routing (3+5+6)→4, 1→2 (duplicate of 24)
 * 
 * This algorithm is identical to Algorithm 24 with mixed routing where:
 * - Operators 3, 5, and 6 in parallel modulate operator 4
 * - Operator 1 modulates operator 2
 * Both operators 2 and 4 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 2 and 4
 */
double FMSynthesizer::processAlgorithm25(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    double out2 = generateOperatorOutput(ops[1], mod1);
    double out4 = generateOperatorOutput(ops[3], mod3 + mod5 + mod6);
    
    return out2 + out4;
}

/**
 * @brief Process Algorithm 26: Mixed routing (1+3+6)→2, 4, 5
 * 
 * This algorithm has mixed routing where:
 * - Operators 1, 3, and 6 in parallel modulate operator 2
 * - Operator 4 acts as a direct carrier
 * - Operator 5 acts as a direct carrier
 * Operators 2, 4, and 5 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 2, 4, and 5
 */
double FMSynthesizer::processAlgorithm26(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double out2 = generateOperatorOutput(ops[1], mod1 + mod3 + mod6);
    double out4 = generateOperatorOutput(ops[3], 0.0);
    double out5 = generateOperatorOutput(ops[4], 0.0);
    
    return out2 + out4 + out5;
}

/**
 * @brief Process Algorithm 27: Mixed routing (1+2+3+5)→4, 6
 * 
 * This algorithm has mixed routing where:
 * - Operators 1, 2, 3, and 5 in parallel modulate operator 4
 * - Operator 6 acts as a direct carrier
 * Both operators 4 and 6 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 4 and 6
 */
double FMSynthesizer::processAlgorithm27(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    double out4 = generateOperatorOutput(ops[3], mod1 + mod2 + mod3 + mod5);
    double out6 = generateOperatorOutput(ops[5], 0.0);
    
    return out4 + out6;
}

/**
 * @brief Process Algorithm 28: Mixed routing (1+2+3+6)→4, 5
 * 
 * This algorithm has mixed routing where:
 * - Operators 1, 2, 3, and 6 in parallel modulate operator 4
 * - Operator 5 acts as a direct carrier
 * Both operators 4 and 5 act as carriers (outputs).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from operators 4 and 5
 */
double FMSynthesizer::processAlgorithm28(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double out4 = generateOperatorOutput(ops[3], mod1 + mod2 + mod3 + mod6);
    double out5 = generateOperatorOutput(ops[4], 0.0);
    
    return out4 + out5;
}

/**
 * @brief Process Algorithm 29: All parallel (1+2+3+4+5)→6 (duplicate of 22)
 * 
 * This algorithm is identical to Algorithm 22 with all operators 1-5 in parallel
 * modulating operator 6. Only operator 6 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 6
 */
double FMSynthesizer::processAlgorithm29(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5);
}

/**
 * @brief Process Algorithm 30: All parallel (1+2+3+4+5)→6 (duplicate of 22)
 * 
 * This algorithm is identical to Algorithm 22 with all operators 1-5 in parallel
 * modulating operator 6. Only operator 6 acts as a carrier (output).
 * 
 * @param voice Reference to the voice containing the operators
 * @return The output sample from operator 6
 */
double FMSynthesizer::processAlgorithm30(Voice& voice) {
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5);
}

/**
 * @brief Process Algorithm 31: All parallel carriers 1, 2, 3, 4, 5, 6
 * 
 * This algorithm has all six operators acting as independent carriers.
 * All operators 1-6 act as carriers (outputs) with no modulation between them.
 * 
 * @param voice Reference to the voice containing the operators
 * @return The combined output from all operators 1-6
 */
double FMSynthesizer::processAlgorithm31(Voice& voice) {
    auto& ops = voice.operators;
    
    double out1 = generateOperatorOutput(ops[0], 0.0);
    double out2 = generateOperatorOutput(ops[1], 0.0);
    double out3 = generateOperatorOutput(ops[2], 0.0);
    double out4 = generateOperatorOutput(ops[3], 0.0);
    double out5 = generateOperatorOutput(ops[4], 0.0);
    double out6 = generateOperatorOutput(ops[5], 0.0);
    
    return out1 + out2 + out3 + out4 + out5 + out6;
}

} // namespace toybasic
