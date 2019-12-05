/*
**	MIT License
**
**	Copyright (c) 2018-2019 Exoway
**
**	Permission is hereby granted, free of charge, to any person obtaining a copy
**	of this software and associated documentation files (the "Software"), to deal
**	in the Software without restriction, including without limitation the rights
**	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
**	copies of the Software, and to permit persons to whom the Software is
**	furnished to do so, subject to the following conditions:
**
**	The above copyright notice and this permission notice shall be included in all
**	copies or substantial portions of the Software.
**
**	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
**	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
**	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
**	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
**	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
**	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
**	SOFTWARE.
**
*/

#include <node.h>
#include "DiffWorkerCallback.hpp"
#include "PatchWorkerCallback.hpp"

extern "C" {
  #include "c/bsdiff/bsdiff.h"
  #include "c/bspatch/bspatch.h"  
}

namespace bsdpNode {
  using namespace v8;

  size_t size = 100;
  char *CharBuff = new char[size + 1];

  void diff(const FunctionCallbackInfo<Value>& args) 
  {
    Isolate* isolate = args.GetIsolate();

    if(args.Length() < 4 || !args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) 
    {
      v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, CharBuff, v8::NewStringType::kNormal, static_cast<int>(size));
      isolate->ThrowException(Exception::Error(result.ToLocalChecked()));
      return;
    }
    
    Nan::Callback *callback = new Nan::Callback(args[3].As<v8::Function>());

    Nan::Utf8String param0(args[0]);
    std::string oldfile = std::string(*param0);  

    Nan::Utf8String param1(args[1]);
    std::string newfile = std::string(*param1);  

    Nan::Utf8String param2(args[2]);
    std::string patchfile = std::string(*param2);  

    DiffWorkerCallback* wc = new DiffWorkerCallback(callback, oldfile, newfile, patchfile);
    Nan::AsyncQueueWorker(wc);
  }

  void diffSync(const FunctionCallbackInfo<Value>& args) 
  {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    
    if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) 
    {
      v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, CharBuff, v8::NewStringType::kNormal, static_cast<int>(size));
      isolate->ThrowException(Exception::Error(result.ToLocalChecked()));
      return;
    }
    
    Nan::Utf8String oldfile(args[0]);
    Nan::Utf8String newfile(args[1]);
    Nan::Utf8String patchfile(args[2]);


    char error[1024];
    memset(error, 0, sizeof error);

    int ret = bsdiff(error, *oldfile, *newfile, *patchfile, nullptr, nullptr); 

    if(ret != 0) {
      v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, CharBuff, v8::NewStringType::kNormal, static_cast<int>(size));
      isolate->ThrowException(Exception::Error(result.ToLocalChecked()));
    }
  }

  void patch(const FunctionCallbackInfo<Value>& args) 
  {
    Isolate* isolate = args.GetIsolate();

    if(args.Length() < 4 || !args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) 
    {
      v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, CharBuff, v8::NewStringType::kNormal, static_cast<int>(size));
      isolate->ThrowException(Exception::Error(result.ToLocalChecked()));
      return;
    }
    
    Nan::Callback *callback = new Nan::Callback(args[3].As<v8::Function>());

    Nan::Utf8String param0(args[0]);
    std::string oldfile = std::string(*param0);  

    Nan::Utf8String param1(args[1]);
    std::string newfile = std::string(*param1);  

    Nan::Utf8String param2(args[2]);
    std::string patchfile = std::string(*param2);  

    PatchWorkerCallback* wc = new PatchWorkerCallback(callback, oldfile, newfile, patchfile);
    Nan::AsyncQueueWorker(wc);
  }

  void patchSync(const FunctionCallbackInfo<Value>& args) 
  {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) 
    {
      v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, CharBuff, v8::NewStringType::kNormal, static_cast<int>(size));
      if (result.IsEmpty()) {
          return;
      }
      isolate->ThrowException(Exception::Error(result.ToLocalChecked()));
      return;
    }
    
    Nan::Utf8String oldfile(args[0]);
    Nan::Utf8String newfile(args[1]);
    Nan::Utf8String patchfile(args[2]);

    char error[1024];
    memset(error, 0, sizeof error);

    int ret = bspatch(error, *oldfile, *newfile, *patchfile, nullptr, nullptr);  

    if(ret != 0) {
      v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, CharBuff, v8::NewStringType::kNormal, static_cast<int>(size));
      if (result.IsEmpty()) {
          return;
      }
      isolate->ThrowException(Exception::Error(result.ToLocalChecked())); 
    }
  }


  void init(Local<Object> exports, Local<Object> module) {
    NODE_SET_METHOD(exports, "diff", diff);
    NODE_SET_METHOD(exports, "diffSync", diffSync);

    NODE_SET_METHOD(exports, "patch", patch);
    NODE_SET_METHOD(exports, "patchSync", patchSync);
  }

  NODE_MODULE(bsdp, init)
}