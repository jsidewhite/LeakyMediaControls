#pragma once

#define THROW_IF_FAILED(x) do{HRESULT hr = S_OK; hr = (x); if (FAILED(hr)){throw 1;}}while(false)
