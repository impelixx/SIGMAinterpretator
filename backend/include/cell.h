#include <string>
#include <vector>


enum class CellType {
    GoToCell,
    ReturnCell,
    CallCeil,
    ConditionalJumpCell,
    LabelCell,
    MathCell,
    VarCell,
    FunctionCell
};


struct RPNCell {
  CellType type;
  std::string value;
  std::string GetTypeAsString() const {
    switch (type) {
      case CellType::GoToCell: return "GoTo";
      case CellType::ReturnCell: return "Return";
      case CellType::VarCell: return "Var";
      case CellType::CallCeil: return "Call";
      case CellType::ConditionalJumpCell: return "ConditionalJump";
      case CellType::LabelCell: return "Label";
      case CellType::MathCell: return "Math";
      case CellType::FunctionCell: return "Function";
      default: return "Unknown";
    }
  }
  RPNCell(CellType t, std::string v) : type(t), value(std::move(v)) {}
};