#pragma once

enum PinRole{

    /// @brief Start the generator
    Start,
    /// @brief Stop the generator
    Stop,
    /// @brief Fire the transfer switch
    Transfer,
    /// @brief Leg 1 of the utility supply
    UtilityOnL1,
    /// @brief Leg 2 of the utility supply
    UtilityOnL2,
    /// @brief Leg 1 of the utility supply
    GeneratorOnL1,
    /// @brief Leg 2 of the utility supply
    GeneratorOnL2,
    /// @brief Some un documented use case, that can be ignored if desired
    Other
};