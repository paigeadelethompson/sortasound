#include "fm.hpp"
#include <cmath>

namespace toybasic {

// Algorithm implementations based on the 32 algorithm diagrams
// All algorithms use 6 operators with various modulation patterns

double FMSynthesizer::processAlgorithm0(Voice& voice) {
    // Algorithm 1: 6->5->4->3->2->1 (serial chain)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    return generateOperatorOutput(ops[0], mod2); // Output from op1
}

double FMSynthesizer::processAlgorithm1(Voice& voice) {
    // Algorithm 2: 5,6->4->3->2->1 (parallel modulators into serial chain)
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod5 + mod6);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    return generateOperatorOutput(ops[0], mod2); // Output from op1
}

double FMSynthesizer::processAlgorithm2(Voice& voice) {
    // Algorithm 3: 6->5->4->3->2, 6->1 (serial chain + parallel modulator)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    double mod1 = generateOperatorOutput(ops[0], mod6);
    return mod2 + mod1; // Output from op2 + op1
}

double FMSynthesizer::processAlgorithm3(Voice& voice) {
    // Algorithm 4: 6->5->4->3, 6->2->1 (two serial chains)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    
    double mod2 = generateOperatorOutput(ops[1], mod6);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod3 + mod1; // Output from op3 + op1
}

double FMSynthesizer::processAlgorithm4(Voice& voice) {
    // Algorithm 5: 6->5->4, 6->3->2->1 (two serial chains of different lengths)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    
    double mod3 = generateOperatorOutput(ops[2], mod6);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod4 + mod1; // Output from op4 + op1
}

double FMSynthesizer::processAlgorithm5(Voice& voice) {
    // Algorithm 6: 6->5, 6->4->3->2->1 (two serial chains)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    
    double mod4 = generateOperatorOutput(ops[3], mod6);
    double mod3 = generateOperatorOutput(ops[2], mod4);
    double mod2 = generateOperatorOutput(ops[1], mod3);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod5 + mod1; // Output from op5 + op1
}

double FMSynthesizer::processAlgorithm6(Voice& voice) {
    // Algorithm 7: 6->5->4, 6->3, 6->2->1 (three serial chains)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod5);
    
    double mod3 = generateOperatorOutput(ops[2], mod6);
    
    double mod2 = generateOperatorOutput(ops[1], mod6);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod4 + mod3 + mod1; // Output from op4 + op3 + op1
}

double FMSynthesizer::processAlgorithm7(Voice& voice) {
    // Algorithm 8: 6->5, 6->4, 6->3, 6->2->1 (four serial chains)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod4 = generateOperatorOutput(ops[3], mod6);
    double mod3 = generateOperatorOutput(ops[2], mod6);
    
    double mod2 = generateOperatorOutput(ops[1], mod6);
    double mod1 = generateOperatorOutput(ops[0], mod2);
    
    return mod5 + mod4 + mod3 + mod1; // Output from op5 + op4 + op3 + op1
}

double FMSynthesizer::processAlgorithm8(Voice& voice) {
    // Algorithm 9: Mixed routing 6->5->3, 4->1, 2->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod5 = generateOperatorOutput(ops[4], mod6);
    double mod3 = generateOperatorOutput(ops[2], mod5);
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod4 + mod2); // Output from op1
}

double FMSynthesizer::processAlgorithm9(Voice& voice) {
    // Algorithm 10: Mixed routing (5+6)->4->1, 3->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod5 + mod6);
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    
    return generateOperatorOutput(ops[0], mod4 + mod3); // Output from op1
}

double FMSynthesizer::processAlgorithm10(Voice& voice) {
    // Algorithm 11: Triple parallel (4+5+6)->3->1, 2->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod5 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2); // Output from op1
}

double FMSynthesizer::processAlgorithm11(Voice& voice) {
    // Algorithm 12: Duplicate of Algorithm 11 - (4+5+6)->3->1, 2->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod5 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2); // Output from op1
}

double FMSynthesizer::processAlgorithm12(Voice& voice) {
    // Algorithm 13: Triple parallel (5+6)->2->1, 4->1, 3->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod5 + mod6);
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    
    return generateOperatorOutput(ops[0], mod2 + mod4 + mod3); // Output from op1
}

double FMSynthesizer::processAlgorithm13(Voice& voice) {
    // Algorithm 14: Duplicate of Algorithm 13 - (5+6)->2->1, 4->1, 3->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod5 + mod6);
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    
    return generateOperatorOutput(ops[0], mod2 + mod4 + mod3); // Output from op1
}

double FMSynthesizer::processAlgorithm14(Voice& voice) {
    // Algorithm 15: Triple parallel (4+6)->3->1, 2->1, 5->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2 + mod5); // Output from op1
}

double FMSynthesizer::processAlgorithm15(Voice& voice) {
    // Algorithm 16: Duplicate of Algorithm 15 - (4+6)->3->1, 2->1, 5->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod4 + mod6);
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[0], mod3 + mod2 + mod5); // Output from op1
}

double FMSynthesizer::processAlgorithm16(Voice& voice) {
    // Algorithm 17: Triple parallel (5+6)->4->1, 3->1, 2->1 (single carrier: 1)
    auto& ops = voice.operators;
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod5 + mod6);
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    
    return generateOperatorOutput(ops[0], mod4 + mod3 + mod2); // Output from op1
}

double FMSynthesizer::processAlgorithm17(Voice& voice) {
    // Algorithm 18: Mixed routing (2+6)->4->5, 1 (single carrier: 5)
    auto& ops = voice.operators;
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod4 = generateOperatorOutput(ops[3], mod2 + mod6);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[4], mod4); // Output from op5
}

double FMSynthesizer::processAlgorithm18(Voice& voice) {
    // Algorithm 19: Triple parallel (3+5+6)->2->4, 1 (single carrier: 4)
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod3 + mod5 + mod6);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod2); // Output from op4
}

double FMSynthesizer::processAlgorithm19(Voice& voice) {
    // Algorithm 20: Mixed routing (3+6)->2->4, 5->4, 1 (single carrier: 4)
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod2 = generateOperatorOutput(ops[1], mod3 + mod6);
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod2 + mod5); // Output from op4
}

double FMSynthesizer::processAlgorithm20(Voice& voice) {
    // Algorithm 21: Mixed routing (2+6)->3->4, 5->4, 1 (single carrier: 4)
    auto& ops = voice.operators;
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod2 + mod6);
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod3 + mod5); // Output from op4
}

double FMSynthesizer::processAlgorithm21(Voice& voice) {
    // Algorithm 22: Duplicate of Algorithm 21 - (2+6)->3->4, 5->4, 1 (single carrier: 4)
    auto& ops = voice.operators;
    
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    double mod3 = generateOperatorOutput(ops[2], mod2 + mod6);
    
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    return generateOperatorOutput(ops[3], mod3 + mod5); // Output from op4
}

double FMSynthesizer::processAlgorithm22(Voice& voice) {
    // Algorithm 23: All parallel (1+2+3+4+5)->6 (single carrier: 6)
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5); // Output from op6
}

double FMSynthesizer::processAlgorithm23(Voice& voice) {
    // Algorithm 24: Duplicate of Algorithm 23 - (1+2+3+4+5)->6 (single carrier: 6)
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5); // Output from op6
}

double FMSynthesizer::processAlgorithm24(Voice& voice) {
    // Algorithm 25: Mixed routing (3+5+6)->4, 1->2 (dual carriers: 2,4)
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    double out2 = generateOperatorOutput(ops[1], mod1); // Output from op2
    double out4 = generateOperatorOutput(ops[3], mod3 + mod5 + mod6); // Output from op4
    
    return out2 + out4; // Dual carriers: 2,4
}

double FMSynthesizer::processAlgorithm25(Voice& voice) {
    // Algorithm 26: Duplicate of Algorithm 25 - (3+5+6)->4, 1->2 (dual carriers: 2,4)
    auto& ops = voice.operators;
    
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    
    double out2 = generateOperatorOutput(ops[1], mod1); // Output from op2
    double out4 = generateOperatorOutput(ops[3], mod3 + mod5 + mod6); // Output from op4
    
    return out2 + out4; // Dual carriers: 2,4
}

double FMSynthesizer::processAlgorithm26(Voice& voice) {
    // Algorithm 27: Mixed routing (1+3+6)->2, 4, 5 (triple carriers: 2,4,5)
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double out2 = generateOperatorOutput(ops[1], mod1 + mod3 + mod6); // Output from op2
    double out4 = generateOperatorOutput(ops[3], 0.0); // Output from op4
    double out5 = generateOperatorOutput(ops[4], 0.0); // Output from op5
    
    return out2 + out4 + out5; // Triple carriers: 2,4,5
}

double FMSynthesizer::processAlgorithm27(Voice& voice) {
    // Algorithm 28: Mixed routing (1+2+3+5)->4, 6 (dual carriers: 4,6)
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    double out4 = generateOperatorOutput(ops[3], mod1 + mod2 + mod3 + mod5); // Output from op4
    double out6 = generateOperatorOutput(ops[5], 0.0); // Output from op6
    
    return out4 + out6; // Dual carriers: 4,6
}

double FMSynthesizer::processAlgorithm28(Voice& voice) {
    // Algorithm 29: Mixed routing (1+2+3+6)->4, 5 (dual carriers: 4,5)
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod6 = generateOperatorOutput(ops[5], 0.0);
    
    double out4 = generateOperatorOutput(ops[3], mod1 + mod2 + mod3 + mod6); // Output from op4
    double out5 = generateOperatorOutput(ops[4], 0.0); // Output from op5
    
    return out4 + out5; // Dual carriers: 4,5
}

double FMSynthesizer::processAlgorithm29(Voice& voice) {
    // Algorithm 30: Duplicate of Algorithm 23 - (1+2+3+4+5)->6 (single carrier: 6)
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5); // Output from op6
}

double FMSynthesizer::processAlgorithm30(Voice& voice) {
    // Algorithm 31: Duplicate of Algorithm 23 - (1+2+3+4+5)->6 (single carrier: 6)
    auto& ops = voice.operators;
    
    double mod1 = generateOperatorOutput(ops[0], 0.0);
    double mod2 = generateOperatorOutput(ops[1], 0.0);
    double mod3 = generateOperatorOutput(ops[2], 0.0);
    double mod4 = generateOperatorOutput(ops[3], 0.0);
    double mod5 = generateOperatorOutput(ops[4], 0.0);
    
    return generateOperatorOutput(ops[5], mod1 + mod2 + mod3 + mod4 + mod5); // Output from op6
}

double FMSynthesizer::processAlgorithm31(Voice& voice) {
    // Algorithm 32: All parallel carriers 1, 2, 3, 4, 5, 6 (six carriers: 1,2,3,4,5,6)
    auto& ops = voice.operators;
    
    double out1 = generateOperatorOutput(ops[0], 0.0); // Output from op1
    double out2 = generateOperatorOutput(ops[1], 0.0); // Output from op2
    double out3 = generateOperatorOutput(ops[2], 0.0); // Output from op3
    double out4 = generateOperatorOutput(ops[3], 0.0); // Output from op4
    double out5 = generateOperatorOutput(ops[4], 0.0); // Output from op5
    double out6 = generateOperatorOutput(ops[5], 0.0); // Output from op6
    
    return out1 + out2 + out3 + out4 + out5 + out6; // Six carriers: 1,2,3,4,5,6
}

} // namespace toybasic
