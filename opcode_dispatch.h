// Automatically generated by generate_ops.py. DO NOT EDIT.

[&](const InsnFunctionParameter& insn) { stepFunctionParameter(insn); },
[&](const InsnFunctionCall& insn) { stepFunctionCall(insn); },
[&](const InsnLoad& insn) { stepLoad(insn); },
[&](const InsnStore& insn) { stepStore(insn); },
[&](const InsnAccessChain& insn) { stepAccessChain(insn); },
[&](const InsnCompositeConstruct& insn) { stepCompositeConstruct(insn); },
[&](const InsnCompositeExtract& insn) { stepCompositeExtract(insn); },
[&](const InsnConvertSToF& insn) { stepConvertSToF(insn); },
[&](const InsnFAdd& insn) { stepFAdd(insn); },
[&](const InsnFSub& insn) { stepFSub(insn); },
[&](const InsnFMul& insn) { stepFMul(insn); },
[&](const InsnFDiv& insn) { stepFDiv(insn); },
[&](const InsnReturn& insn) { stepReturn(insn); },
