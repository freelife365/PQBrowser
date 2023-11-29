### 头文件包含顺序
1. 系统头文件 OS SDK.h 
2. C system files. 
3. C++ system files.
4. Other libraries' .h files.
5. Your project's .h files.


举个例子：
For example, the includes in google-awesome-project/src/foo/internal/fooserver.cc might look like this:
```cpp
#include "foo/public/fooserver.h"  // Preferred location.

#include <sys/types.h>

#include <unistd.h>

#include <hash_map>

#include <vector>

#include "base/basictypes.h"

#include "base/commandlineflags.h"

#include "foo/public/bar.h"
```
