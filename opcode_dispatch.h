// Automatically generated by generate_ops.py. DO NOT EDIT.

[&](const InsnFunctionParameter& insn) { stepFunctionParameter(insn); },
[&](const InsnFunctionCall& insn) { stepFunctionCall(insn); },
[&](const InsnLoad& insn) { stepLoad(insn); },
[&](const InsnStore& insn) { stepStore(insn); },
[&](const InsnAccessChain& insn) { stepAccessChain(insn); },
[&](const InsnVectorShuffle& insn) { stepVectorShuffle(insn); },
[&](const InsnCompositeConstruct& insn) { stepCompositeConstruct(insn); },
[&](const InsnCompositeExtract& insn) { stepCompositeExtract(insn); },
[&](const InsnConvertFToS& insn) { stepConvertFToS(insn); },
[&](const InsnConvertSToF& insn) { stepConvertSToF(insn); },
[&](const InsnFNegate& insn) { stepFNegate(insn); },
[&](const InsnIAdd& insn) { stepIAdd(insn); },
[&](const InsnFAdd& insn) { stepFAdd(insn); },
[&](const InsnFSub& insn) { stepFSub(insn); },
[&](const InsnFMul& insn) { stepFMul(insn); },
[&](const InsnFDiv& insn) { stepFDiv(insn); },
[&](const InsnFMod& insn) { stepFMod(insn); },
[&](const InsnVectorTimesScalar& insn) { stepVectorTimesScalar(insn); },
[&](const InsnDot& insn) { stepDot(insn); },
[&](const InsnLogicalNot& insn) { stepLogicalNot(insn); },
[&](const InsnSelect& insn) { stepSelect(insn); },
[&](const InsnIEqual& insn) { stepIEqual(insn); },
[&](const InsnSLessThan& insn) { stepSLessThan(insn); },
[&](const InsnFOrdEqual& insn) { stepFOrdEqual(insn); },
[&](const InsnFOrdLessThan& insn) { stepFOrdLessThan(insn); },
[&](const InsnFOrdGreaterThan& insn) { stepFOrdGreaterThan(insn); },
[&](const InsnFOrdLessThanEqual& insn) { stepFOrdLessThanEqual(insn); },
[&](const InsnFOrdGreaterThanEqual& insn) { stepFOrdGreaterThanEqual(insn); },
[&](const InsnPhi& insn) { stepPhi(insn); },
[&](const InsnBranch& insn) { stepBranch(insn); },
[&](const InsnBranchConditional& insn) { stepBranchConditional(insn); },
[&](const InsnReturn& insn) { stepReturn(insn); },
[&](const InsnReturnValue& insn) { stepReturnValue(insn); },
[&](const InsnGLSLstd450FAbs& insn) { stepGLSLstd450FAbs(insn); },
[&](const InsnGLSLstd450Floor& insn) { stepGLSLstd450Floor(insn); },
[&](const InsnGLSLstd450Sin& insn) { stepGLSLstd450Sin(insn); },
[&](const InsnGLSLstd450Cos& insn) { stepGLSLstd450Cos(insn); },
[&](const InsnGLSLstd450FMin& insn) { stepGLSLstd450FMin(insn); },
[&](const InsnGLSLstd450FMax& insn) { stepGLSLstd450FMax(insn); },
[&](const InsnGLSLstd450Distance& insn) { stepGLSLstd450Distance(insn); },
[&](const InsnGLSLstd450Normalize& insn) { stepGLSLstd450Normalize(insn); },
