# Project2 빌드 안내

## 다른 컴퓨터에서 열기

1. Visual Studio Installer에서 **C++를 사용한 데스크톱 개발** 워크로드를 설치합니다. 이 프로젝트는 `v145` 도구 집합을 사용하므로 이를 지원하는 Visual Studio와 Windows SDK가 필요합니다.
2. Installer의 개별 구성 요소에서 **vcpkg 패키지 관리자**가 설치되어 있는지 확인합니다.
3. GitHub Desktop으로 저장소를 Clone 또는 Pull합니다.
4. `Project2.slnx`를 열고 **Debug / x64**로 빌드합니다. 최초 빌드에는 인터넷 연결이 필요합니다.

프로젝트가 `vcpkg.json`을 읽어 GLEW, GLFW, GLM을 자동 설치하고 포함 경로와 링크 설정을 적용합니다. 컴퓨터별 절대 경로를 추가할 필요는 없습니다.

## GLM 사용

```cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
```

의존성 버전 기준은 `vcpkg.json`의 `builtin-baseline`으로 고정합니다. GitHub Desktop으로 의존성 변경을 공유할 때는 **vcpkg.json을 커밋**하세요. 자동 생성되는 `vcpkg_installed`와 빌드 결과는 `.gitignore`에 따라 제외합니다.

빌드 이후에도 편집기에 헤더 오류가 남으면 프로젝트를 다시 로드하거나 Visual Studio를 다시 열어 보세요.

## 실습 파일 선택

현재 Debug/x64 구성은 `1-12.cpp`를 빌드합니다. 다른 실습 파일을 실행하려면 해당 파일을 프로젝트에 추가하고, `main()`이 있는 기존 실습 파일은 **빌드에서 제외**하세요. 한 실행 프로그램에는 `main()`이 하나만 있어야 합니다.
