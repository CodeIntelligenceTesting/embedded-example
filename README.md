# Embedded fuzzing example

This project is a simple demo on fuzzing a cryptographic library that could be used in embedded systems.

## How to Onboard

#### Initialize the repo
The very first step you need to do is run the following command to initialize cifuzz:
```
cifuzz init
```

This command sets up the necessary directory for cifuzz and creates a `cifuzz.yaml` file, which contains the configuration for cifuzz.

Next, you need to enable fuzz testing by adding the following lines to your CMake configuration:
```
find_package(cifuzz NO_SYSTEM_ENVIRONMENT_PATH)
enable_fuzz_testing()
```

#### Create a fuzz test

The next step is to create a fuzz test. You can do this by running the following command:
```
cifuzz create -o fuzztest.cpp
```

This will create an empty fuzz test in the current directory named `fuzztest.cpp`

After that, you will need to add a new target in CMake by including the following line:
```
add_fuzz_test(fuzztest fuzztest.cpp)
```


#### Linking the fuzz test with the software under test

Now that we have an (empty) fuzz test, we want to start fuzzing. Before deciding which function to fuzz, we need to link our target, `fuzztest`, against the library we want to test, `automotive` (as specified in `src/CMakeLists.txt`). To do this, add the following line under the `fuzztest` target in your CMake configuration:
```
target_link_libraries(fuzztest automotive)
```
Once this is added, we can focus on the fuzz test. In this case, we have multiple modules we could fuzz (located in `src`), but we will focus on the `crypto`    library. First, we need to include the respective header within an extern `C` block to allow us to fuzz the targeted function:

```cpp
extern "C" {
#include "crypto_1.h"
}
```

If you try to run it with:

```
cifuzz run fuzztest
```

you should encounter a linker error. This is likely because some functions are not defined here and would typically come from third-party code or drivers. This situation can occur frequently in embedded projects. A common approach to resolve this is to create dummy implementations of these functions for now. You can create a new file named `mocks.c` and add it to the CMake configuration for the `automotive` library.

For example, here’s a simple implementation for the undefined function `driver_get_current_time`:
```c
int driver_get_current_time() {
  return 0;
}
```

### Fuzzing the project

Now we can finally focus on the fuzz test. Let's say we want to fuzz the `crypto_verify_hmac()` function. We see that it takes three arguments:
```c
const uint8_t *message, int len, crypto_hmac *hmac
```

This is a great target for the fuzzer. We want to send fuzz-generated data to this function through these parameters. To do this, we can use the `FuzzDataProvider` helper to obtain data in the exact types we need within the `FUZZ_TEST` body:

```cpp
// Generate a random length for the message
int message_len = fuzzed_data.ConsumeIntegralInRange<int>(0, 1024);

// Generate the message based on the length
std::vector<uint8_t> message = fuzzed_data.ConsumeBytes<uint8_t>(message_len);

// Create a crypto_hmac struct and fill it with fuzz-generated data
crypto_hmac hmac;
std::vector<uint8_t> hmac_data = fuzzed_data.ConsumeBytes<uint8_t>(sizeof(hmac.hmac));
std::copy(hmac_data.begin(), hmac_data.end(), hmac.hmac);
```

Once this is added, we can finally call the function with the variables we created:
```cpp
crypto_verify_hmac(message.data(), message.size(), &hmac);
```

You can then fuzz the project using:
```
cifuzz run fuzztest
```

### Coverage & improvement to the fuzz test
You can check the coverage by running:
```
cifuzz coverage fuzztest
```

As you may notice, we are not able to reach much code, as the fuzzer seems to be stuck (in `src/crypto/crypto_1.c`). This indicates that we need to better understand the code and improve the fuzz test. You can try this on your own or refer to the "solution" on the other branch: `fuzzing-setup`
