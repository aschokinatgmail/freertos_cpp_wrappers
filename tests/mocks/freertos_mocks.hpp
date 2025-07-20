#pragma once

// FreeRTOS mocks header for C++ unit testing
// This header provides the mock class and instance management for FreeRTOS functions

#include "FreeRTOS.h"
#include <memory>
#include <gmock/gmock.h>

// Mock class for FreeRTOS functions (already defined in FreeRTOS.h)
// This file provides instance management utilities

class FreeRTOSMockInstance {
public:
    static std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> getInstance() {
        if (!instance_) {
            instance_ = std::make_shared<::testing::StrictMock<::FreeRTOSMock>>();
            setGlobalInstance(instance_.get());
        }
        return instance_;
    }
    
    static void resetInstance() {
        setGlobalInstance(nullptr);
        instance_.reset();
    }
    
private:
    static void setGlobalInstance(::FreeRTOSMock* mock);
    static std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> instance_;
};