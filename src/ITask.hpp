#pragma once

#include <vector>

using ElementType = double;
using VectorType = std::vector<ElementType>;
using VectorIterator = VectorType::iterator;
using TaskType = ElementType(VectorIterator, VectorIterator, ElementType);

class ITask
{
public:
    virtual ElementType execute() = 0;

    virtual ~ITask() = default;
};
