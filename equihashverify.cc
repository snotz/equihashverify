#include <nan.h>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <stdint.h>
#include <sodium.h>
#include "crypto/equihash.h"

#include "arith_uint256.h"
#include "uint256.h"
#include <vector>
//#include <boost/test/unit_test.hpp>

/*extern "C" {
    #include "src/equi/equi.h"
}*/

using namespace v8;

bool verifyEH(const char *hdr, const char *soln){
  unsigned int n = 200;
  unsigned int k = 9;

  // Hash state
  crypto_generichash_blake2b_state state;
  EhInitialiseState(n, k, state);

  crypto_generichash_blake2b_update(&state, (const unsigned char*)hdr, 140);

  std::vector<unsigned char>::size_type size = strlen(soln);
  std::vector<unsigned char> vec(soln, soln + size);
  bool isValid = Eh200_9.IsValidSolution(state, vec);
  
  return isValid;
}

void Verify(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 2) {
  isolate->ThrowException(Exception::TypeError(
    String::NewFromUtf8(isolate, "Wrong number of arguments")));
  return;
  }

  Local<Object> header = args[0]->ToObject();
  Local<Object> solution = args[1]->ToObject();

  if(!node::Buffer::HasInstance(header) || !node::Buffer::HasInstance(solution)) {
  isolate->ThrowException(Exception::TypeError(
    String::NewFromUtf8(isolate, "Arguments should be buffer objects.")));
  return;
  }

  const char *hdr = node::Buffer::Data(header);
  const char *soln = node::Buffer::Data(solution);

  bool result = verifyEH(hdr, soln);
  args.GetReturnValue().Set(result);

}


void Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "verify", Verify);
}

NODE_MODULE(equihashverify, Init)