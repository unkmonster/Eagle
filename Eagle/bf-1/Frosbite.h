#pragma once

#include <Windows.h>
#include <cassert>
#include <cstdint>
#include <string>
#include <vector> 
#include <algorithm>

#include <imgui.h>
  
#include "render/Sprite2d.h"
#define NOD3DX9

#ifdef NOD3DX9
#include "math/Vector.h"
using D3DXMATRIXA16 = Matrix16;
using D3DXVECTOR4 = Vec4;
using D3DXVECTOR3 = Vec3;
#else
#include <d3dx9math.h>
#endif // NOD3DX9

#define OFFSET_ObfuscationMgr 0x14351D058
#define OFFSET_CLIENTGAMECONTEXT 0x1437F7758
#define OFFSET_GAMERENDERER 0x1439e6d08
#define ValidPointer( pointer ) ( pointer != NULL && (DWORD_PTR)pointer >= 0x10000 && (DWORD_PTR)pointer < 0x00007FFFFFFEFFFF /*&& some other checks*/ )
void* DecryptPointer(DWORD64 EncryptedPtr, DWORD64 PointerKey);

#define MAX_PLAYERS 64

namespace fb
{
inline Vec3 Mat3Vec(const Matrix16& matrix, const Vec3& vec) {
    return {
        matrix.data[0][0] * vec.x + matrix.data[0][1] * vec.x + matrix.data[0][2] * vec.x,
        matrix.data[1][0] * vec.y + matrix.data[1][1] * vec.y + matrix.data[1][2] * vec.y,
       matrix.data[2][0] * vec.z + matrix.data[2][1] * vec.z + matrix.data[2][2] * vec.z
    };
}

template <class T, INT Count, INT PadSize>
class fixed_vector {
public:
    T* m_firstElement;
    T* m_lastElement;
    T* m_arrayBound;
    LPVOID m_pad[PadSize];
    T m_data[Count];

public:
    fixed_vector() {
        m_firstElement = (T*)m_data;
        m_lastElement = (T*)m_data;
        m_arrayBound = (T*)&m_data[Count];
    }

    void push_back(T* const value) {
        if (m_lastElement > m_arrayBound) {

            return;
        }
        *m_lastElement = *value;
        m_lastElement = m_lastElement + 1;
    };

    void clear() {
        m_firstElement = m_data;
        m_lastElement = m_data;
        m_arrayBound = (T*)&m_data[Count];
    }

    UINT Size() {
        return ((DWORD_PTR)m_lastElement - (DWORD_PTR)m_firstElement) / sizeof(T);
    }

    T At(INT nIndex) {
        return m_firstElement[nIndex];
    }

    T operator [](INT index) { return At(index); }
};

template <class T>
class vector {
private:
    T* m_firstElement;
    T* m_lastElement;
    T* m_arrayBound;
    void* vftable;
public:
    size_t size() {
        return (((intptr_t)this->m_lastElement - (intptr_t)this->m_firstElement) / sizeof(T));
    }
    T at(int nIndex) {
        return *(T*)((intptr_t)m_firstElement + (nIndex * sizeof(T)));
    }
    T operator [](int index) {
        return at(index);
    }
};

template <typename T>
class Array {
private:
    T* m_firstElement;
public:
    T At(INT nIndex) {
        if (!ValidPointer(m_firstElement))
            return (T)NULL;

        return *(T*)((DWORD64)m_firstElement + (nIndex * sizeof(T)));
    };

    T operator [](INT index) { return m_firstElement[index]; }

    int GetSize() {
        if (m_firstElement)
        {
            return *(int*)((DWORD_PTR)m_firstElement - sizeof(__int32));
        }
        return 0;
    }
};

template <typename T>
class Tuple2 {
public:
    T Element1;
    T Element2;

public:
    Tuple2(T _Element1, T _Element2) {
        Element1 = _Element1;
        Element2 = _Element2;
    }
};

class Color32 {
public:
    Color32(DWORD dwColor) {
        this->a = (dwColor >> 24) & 0xff;
        this->r = (dwColor >> 16) & 0xff;
        this->g = (dwColor >> 8) & 0xff;
        this->b = (dwColor) & 0xff;
    }

    Color32(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) :
        r(red), g(green), b(blue), a(alpha) {}

    static Color32 Yellow() { return Color32(255, 255, 000, 255); }
    static Color32 White() { return Color32(255, 255, 255, 255); }
    static Color32 Black() { return Color32(0, 0, 0, 255); }
    static Color32 Red() { return Color32(255, 0, 0, 255); }
    static Color32 Green() { return Color32(0, 255, 0, 255); }
    static Color32 Blue() { return Color32(0, 0, 255, 255); }
private:
    union {
        struct {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };

        unsigned long color;
    };
};

template< class T >
struct WeakToken {
    T* m_realptr; //its value is: realptr + 8
    DWORD32 m_refcount;
};

template< class T >
class WeakPtr {
private:
    WeakToken<T>* m_ptr;
public:
    T* GetData() {
        if (!ValidPointer(m_ptr))
            return NULL;

        if (!ValidPointer(&m_ptr->m_realptr))
            return NULL;

        T* ptr = m_ptr->m_realptr;
        if (!ValidPointer(ptr))
            return NULL;

        return (T*)((DWORD_PTR)(ptr)-0x8);
    }
};

template< class T >
class EncryptedPtr {
public:
    DWORD64 m_encryptedPtr;
    DWORD64 m_pointerKey;

public:
    T* GetPtr() {
        return (T*)(DecryptPointer(this->m_encryptedPtr, (DWORD64)(this->m_pointerKey)));
    }
};

typedef D3DXMATRIXA16 LinearTransform;
class AxisAlignedBox {
public:
    D3DXVECTOR4 min;
    D3DXVECTOR4 max;
};

struct LinearTransform_AABB {
public:
    LinearTransform m_Transform;
    AxisAlignedBox m_Box;
    char _pad[0x200];
};

class ClientPlayerManager {
public:

};

class ClientGameContext {
public:
    char pad_0x0000[0x68]; //0x0000
    ClientPlayerManager* m_clientPlayerManager; //0x0068 
    static ClientGameContext* GetInstance() {
        return *(ClientGameContext**)(OFFSET_CLIENTGAMECONTEXT);
    }
};

class HealthComponent {
public:
    char pad_0000[32]; //0x0000
    float m_Health; //0x0020
    float m_MaxHealth; //0x0024
    char pad_0028[24]; //0x0028
    float m_VehicleHealth; //0x0040
    char pad_0044[4092]; //0x0044
}; //Size: 0x1040

class VehicleEntityData {
public:
    char pad_0000[504]; //0x0000
    float m_FrontMaxHealth; //0x01F8
    char pad_01FC[252]; //0x01FC
    char* m_namesid; //0x02F8
    char pad_0300[7488]; //0x0300
}; //Size: 0x2040

class ClientEntity {
public:
    virtual void Function0(); //
    virtual void Function1(); //
    virtual void Function2(); //
    virtual void Function3(); //
    virtual void Function4(); //
    virtual void Function5(); //
    virtual void Function6(); //
    virtual void Function7(); //
    virtual void Function8(); //
    virtual void Function9(); //
    virtual void Function10(); //
    virtual void Function11(); //
    virtual void Function12(); //
    virtual void Function13(); //
    virtual void Function14(); //
    virtual void Function15(); //
    virtual void Function16(); //
    virtual void Function17(); //
    virtual void Function18(); //
    virtual void Function19(); //
    virtual void Function20(); //
    virtual void Function21(); //
    virtual void Function22(); //
    virtual void Function23(); // 
    virtual void Function24();
    virtual void GetTransformAABB(LinearTransform_AABB& mTransform) const;// 26
    virtual void GetTransform(Matrix16* mTransform) const;
};

class ClientVehicleEntity: public ClientEntity {
public:
    HealthComponent* GetHealthComponent() {
        return *(HealthComponent**)((BYTE*)this + 0x1D0);
    };

    VehicleEntityData* GetEntityData() {
        return *(VehicleEntityData**)((BYTE*)this + 0x30);
    };

    Vec3 GetVehiclePosition() {
        LinearTransform_AABB _AABB;
        Matrix16 _Transform;
        GetTransform(&_Transform);
        Vec3 Position = Vec3(_Transform.data[3][0], _Transform.data[3][1], _Transform.data[3][2]);
        Vec3 glmPos = Vec3(Position.x, Position.y, Position.z);

        GetTransformAABB(_AABB);
        Vec3 min = Vec3(_AABB.m_Box.min.x, _AABB.m_Box.min.y, _AABB.m_Box.min.z);
        Vec3 max = Vec3(_AABB.m_Box.max.x, _AABB.m_Box.max.y, _AABB.m_Box.max.z);

        Matrix16 TransformMatrix{_Transform.data[0][0], _Transform.data[1][0], _Transform.data[2][0], 0,
                                    _Transform.data[0][1], _Transform.data[1][1], _Transform.data[2][1], 0,
                                        _Transform.data[0][2], _Transform.data[1][2], _Transform.data[2][2], 0};


        min = glmPos + Mat3Vec(TransformMatrix, min);
        max = glmPos + Mat3Vec(TransformMatrix, max);

        return Vec3(min.x, min.y, min.z);
    }
}; //Size: 0x0048

class UpdatePoseResultData {
public:
    class QuatTransform {
    public:
        D3DXVECTOR4 m_TransAndScale; //0x0000 
        D3DXVECTOR4 m_Rotation; //0x0010 
    };//Size=0x0020
    char pad_0000[24]; //0x0000
    class QuatTransform* m_ActiveWorldTransforms; //0x0020 
    class QuatTransform* m_ActiveLocalTransforms; //0x0028
    int m_Slot; //0x0030 
    int m_ReaderIndex; //0x0034 
    bool m_ValidTransforms; //0x0038 
    bool m_PoseUpdateEnabled; //0x0039 
    bool m_PoseNeeded; //0x003A 
}; //Size: 0x1040

class BoneCollisionComponent {
public:
    UpdatePoseResultData m_ragdollTransforms; //0x0000
}; //Size: 0x0008

class ClientSoldierEntity:public ClientEntity {
public:
    char pad_0000[456]; //0x0000
    HealthComponent* healthcomponent; //0x01D0
    char pad_01D8[696]; //0x01D8
    BoneCollisionComponent* bonecollisioncomponent; //0x0490
    char pad_0498[363]; //0x0498
    uint8_t N00000670; //0x0603
    float authorativeYaw; //0x0604
    char pad_0608[41]; //0x0608
    uint8_t N00000521; //0x0631
    char pad_0632[6]; //0x0632
    uint8_t poseType; //0x0638
    char pad_0639[176]; //0x0639
    uint8_t N00000538; //0x06E9
    uint8_t N0000022B; //0x06EA
    uint8_t occluded; //0x06EB
    char pad_06EC[669]; //0x06EC
    uint8_t N0000058C; //0x0989
    char pad_098A[6]; //0x098A
    D3DXVECTOR3 location; //0x0990
    char pad_099C[1712]; //0x099C

    bool IsValid() {
        return (this->healthcomponent->m_Health > 0.1f && this->healthcomponent->m_Health <= this->healthcomponent->m_MaxHealth);
    }
    bool IsDead() {
        return !(this->healthcomponent->m_Health > 0.1f);
    }
    bool IsVisible() {
        return (this->occluded == false);
    }

    AxisAlignedBox GetAABB() {
        AxisAlignedBox aabb = AxisAlignedBox();
        if (this->poseType == 0) // standing
        {
            aabb.min = D3DXVECTOR4(-0.350000f, 0.000000f, -0.350000f, 0);
            aabb.max = D3DXVECTOR4(0.350000f, 1.700000f, 0.350000f, 0);
        }
        if (this->poseType == 1) // crouching
        {
            aabb.min = D3DXVECTOR4(-0.350000f, 0.000000f, -0.350000f, 0);
            aabb.max = D3DXVECTOR4(0.350000f, 1.150000f, 0.350000f, 0);
        }
        if (this->poseType == 2) // prone
        {
            aabb.min = D3DXVECTOR4(-0.350000f, 0.000000f, -0.350000f, 0);
            aabb.max = D3DXVECTOR4(0.350000f, 0.400000f, 0.350000f, 0);
        }
        return aabb;
    }
    bool GetBonePos(int BoneId, D3DXVECTOR3* vOut);


}; //Size: 0x104C

class ClientPlayer {
public:
    virtual ~ClientPlayer();
    virtual DWORD_PTR GetCharacterEntity() const; //=> ClientSoldierEntity + 0x268 
    virtual DWORD_PTR GetCharacterUserData() const; //=> PlayerCharacterUserData
    virtual class EntryComponent* GetEntryComponent() const;
    virtual bool InVehicle() const;
    virtual unsigned int getId() const;
    char _0x0008[16];
    char* name; //0x0018
    char pad_0020[32]; //0x0020
    char szName[8]; //0x0040
    char pad_0048[7144]; //0x0048
    uint8_t N00000393; //0x1C30
    uint8_t N0000042C; //0x1C31
    char pad_1C32[2]; //0x1C32
    uint8_t teamId; //0x1C34
    char pad_1C35[259]; //0x1C35
    ClientVehicleEntity* clientVehicleEntity; //0x1D38
    char pad_1D40[8]; //0x1D40
    ClientSoldierEntity* clientSoldierEntity; //0x1D48
    char pad_1D50[736]; //0x1D50
};

class RenderView {
public:
    char pad_0000[1120]; //0x0000
    D3DXMATRIXA16 viewProj; //0x0460
    char pad_04A0[3040]; //0x04A0

}; //Size: 0x05C0

class GameRenderer {
public:
    char pad_0000[96]; //0x0000
    class RenderView* renderView; //0x0060
    char pad_0068[4112]; //0x0068

    static GameRenderer* GetInstance() {
        return *(GameRenderer**)OFFSET_GAMERENDERER;
    }
}; //Size: 0x0088

enum BoneIds {
    BONE_Head = 0x35,
    BONE_Neck = 0x33,
    BONE_SPINE2 = 0x7,
    BONE_SPINE1 = 0x6,
    BONE_Spine = 0x5,
    BONE_LeftShoulder = 0x8,
    BONE_RightShoulder = 0xA3,
    BONE_LeftElbowRoll = 0xE,
    BONE_RightElbowRoll = 0xA9,
    BONE_LeftHand = 0x10,
    BONE_RightHand = 0xAB,
    BONE_RightKneeRoll = 0x12D,
    BONE_LeftKneeRoll = 0x11F,
    BONE_LeftFoot = 0x115,
    BONE_RightFoot = 0x123
};

inline bool w2s(Vec3* in, Vec2* out) {
    auto renderer = GameRenderer::GetInstance();
    assert(ValidPointer(renderer));
    assert(ValidPointer(renderer->renderView));
    auto pViewProjection = &renderer->renderView->viewProj;

    float w = pViewProjection->data[0][3] * in->x + pViewProjection->data[1][3] * in->y + pViewProjection->data[2][3] * in->z + pViewProjection->data[3][3];
    if (w < 0.01)
        return false;

    out->x = 
        pViewProjection->data[0][0] * in->x +
        pViewProjection->data[1][0] * in->y +
        pViewProjection->data[2][0] * in->z +
        pViewProjection->data[3][0];
    out->y = 
        pViewProjection->data[0][1] * in->x +
        pViewProjection->data[1][1] * in->y +
        pViewProjection->data[2][1] * in->z +
        pViewProjection->data[3][1];

    float invw = 1.0f / w;
    out->x *= invw;
    out->y *= invw;

    float width = ImGui::GetIO().DisplaySize.x, height = ImGui::GetIO().DisplaySize.y;
    float x = width / 2.f;
    float y = height / 2.f;

    x += 0.5f * out->x * width + 0.5f;
    y -= 0.5f * out->y * height + 0.5f;
    out->x = x;
    out->y = y;
    return true;
}

inline bool GetBoxPosition(const LinearTransform_AABB& aabb, std::vector<Vec2>& points, std::pair<Vec2, Vec2>& minmax, Vec3& origin) {
    const Vec3& min = aabb.m_Box.min;
    const Vec3& max = aabb.m_Box.max;
    origin = aabb.m_Transform.data[3];
    const std::size_t size = 8;

    // ªÒ»° 3d ∂•µ„
    std::vector<Vec3> apexes;
    apexes.reserve(size);
    apexes.emplace_back(origin + min * aabb.m_Transform);
    apexes.emplace_back(origin + max * aabb.m_Transform);
    apexes.emplace_back(origin + Vec3{min.x, min.y, max.z} *aabb.m_Transform);
    apexes.emplace_back(origin + Vec3{max.x, max.y, min.z} *aabb.m_Transform);

    apexes.emplace_back(origin + Vec3{max.x, min.y, min.z} *aabb.m_Transform);
    apexes.emplace_back(origin + Vec3{min.x, max.y, min.z} *aabb.m_Transform);
    apexes.emplace_back(origin + Vec3{max.x, min.y, max.z} *aabb.m_Transform);
    apexes.emplace_back(origin + Vec3{min.x, max.y, max.z} *aabb.m_Transform);

    // 3d -> 2d
    points.resize(size);
    for (int i = 0; i < size; ++i) {
        if (!w2s(&apexes[i], &points[i])) return false;
#ifdef _DEBUG
        //ImGui::GetForegroundDrawList()->AddText(points[i], 0xffffffff, std::to_string(i).c_str());
#endif
    }

    auto result = std::minmax_element(points.begin(), points.end(), 
        [](const Vec2& lhs, const Vec2& rhs) {
        return distance(lhs, Vec2{0, 0}) < distance(rhs, Vec2{0, 0});
    });

    minmax.first = *result.first;
    minmax.second = *result.second;
    return true;
}
} // fb