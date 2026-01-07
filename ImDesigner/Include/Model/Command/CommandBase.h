#pragma once
#include <memory>
#include <vector>
#include <string>

class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual std::string GetDescription() const = 0;
    virtual bool MergeWith(ICommand* other) { return false; }
};