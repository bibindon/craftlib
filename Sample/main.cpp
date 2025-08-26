#pragma comment( lib, "d3d9.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib")
#else
#pragma comment( lib, "d3dx9.lib")
#endif

#pragma comment (lib, "winmm.lib")

#pragma comment( lib, "craftlib.lib")

#include "..\craftlib\CraftLib.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <tchar.h>

using namespace NSCraftLib;

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

class Sprite : public ISprite
{
public:

    Sprite(LPDIRECT3DDEVICE9 dev)
        : m_pD3DDevice(dev)
    {
    }

    void DrawImage(const int x, const int y, const int transparency) override
    {
        D3DXVECTOR3 pos {(float)x, (float)y, 0.f};
        m_D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
        RECT rect = {
            0,
            0,
            static_cast<LONG>(m_width),
            static_cast<LONG>(m_height) };
        D3DXVECTOR3 center { 0, 0, 0 };
        m_D3DSprite->Draw(
            m_pD3DTexture,
            &rect,
            &center,
            &pos,
            D3DCOLOR_ARGB(transparency, 255, 255, 255));
        m_D3DSprite->End();

    }

    void Load(const std::wstring& filepath) override
    {
        LPD3DXSPRITE tempSprite { nullptr };
        if (FAILED(D3DXCreateSprite(m_pD3DDevice, &m_D3DSprite)))
        {
            throw std::exception("Failed to create a sprite.");
        }

        if (FAILED(D3DXCreateTextureFromFile(
            m_pD3DDevice,
            filepath.c_str(),
            &m_pD3DTexture)))
        {
            throw std::exception("Failed to create a texture.");
        }

        D3DSURFACE_DESC desc { };
        if (FAILED(m_pD3DTexture->GetLevelDesc(0, &desc)))
        {
            throw std::exception("Failed to create a texture.");
        }
        m_width = desc.Width;
        m_height = desc.Height;
    }

    ~Sprite()
    {
        m_D3DSprite->Release();
        m_pD3DTexture->Release();
    }

private:

    LPDIRECT3DDEVICE9 m_pD3DDevice = NULL;
    LPD3DXSPRITE m_D3DSprite = NULL;
    LPDIRECT3DTEXTURE9 m_pD3DTexture = NULL;
    UINT m_width { 0 };
    UINT m_height { 0 };
};

class Font : public IFont
{
public:

    Font(LPDIRECT3DDEVICE9 pD3DDevice)
        : m_pD3DDevice(pD3DDevice)
    {
    }

    void Init(const bool bEnglish)
    {
        if (!bEnglish)
        {
            HRESULT hr = D3DXCreateFont(m_pD3DDevice,
                                        24,
                                        0,
                                        FW_NORMAL,
                                        1,
                                        false,
                                        SHIFTJIS_CHARSET,
                                        OUT_TT_ONLY_PRECIS,
                                        ANTIALIASED_QUALITY,
                                        FF_DONTCARE,
                                        _T("ＭＳ 明朝"),
                                        &m_pFont);
        }
        else
        {
            HRESULT hr = D3DXCreateFont(m_pD3DDevice,
                                        24,
                                        0,
                                        FW_NORMAL,
                                        1,
                                        false,
                                        DEFAULT_CHARSET,
                                        OUT_TT_ONLY_PRECIS,
                                        CLEARTYPE_NATURAL_QUALITY,
                                        FF_DONTCARE,
                                        L"Calibri",
                                        &m_pFont);
        }
    }

    virtual void DrawText_(const std::wstring& msg, const int x, const int y)
    {
        RECT rect = { x, y, 0, 0 };
        m_pFont->DrawText(NULL, msg.c_str(), -1, &rect, DT_LEFT | DT_NOCLIP,
            D3DCOLOR_ARGB(255, 255, 255, 255));
    }

    ~Font()
    {
        m_pFont->Release();
    }

private:

    LPDIRECT3DDEVICE9 m_pD3DDevice = NULL;
    LPD3DXFONT m_pFont = NULL;
};


class SoundEffect : public ISoundEffect
{
    virtual void PlayMove() override
    {
        PlaySound(_T("cursor_move.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    virtual void PlayClick() override
    {
        PlaySound(_T("cursor_confirm.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    virtual void PlayBack() override
    {
        PlaySound(_T("cursor_cancel.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    virtual void Init() override
    {

    }
};

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPD3DXFONT g_pFont = NULL;
LPD3DXMESH pMesh = NULL;
D3DMATERIAL9* pMaterials = NULL;
LPDIRECT3DTEXTURE9* pTextures = NULL;
DWORD dwNumMaterials = 0;
LPD3DXEFFECT pEffect = NULL;
D3DXMATERIAL* d3dxMaterials = NULL;
float f = 0.0f;
bool bShowMenu = true;

CraftLib menu;

void TextDraw(LPD3DXFONT pFont, wchar_t* text, int X, int Y)
{
    RECT rect = { X,Y,0,0 };
    pFont->DrawText(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
}

HRESULT InitD3D(HWND hWnd)
{
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        return E_FAIL;
    }

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
    {
        if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
        {
            return(E_FAIL);
        }
    }

    HRESULT hr = D3DXCreateFont(
        g_pd3dDevice,
        20,
        0,
        FW_HEAVY,
        1,
        false,
        SHIFTJIS_CHARSET,
        OUT_TT_ONLY_PRECIS,
        ANTIALIASED_QUALITY,
        FF_DONTCARE,
        _T("ＭＳ ゴシック"),
        &g_pFont);
    if FAILED(hr)
    {
        return(E_FAIL);
    }

    LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

    if (FAILED(D3DXLoadMeshFromX(_T("cube.x"), D3DXMESH_SYSTEMMEM,
        g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL,
        &dwNumMaterials, &pMesh)))
    {
        MessageBox(NULL, _T("Xファイルの読み込みに失敗しました"), NULL, MB_OK);
        return E_FAIL;
    }
    d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    pMaterials = new D3DMATERIAL9[dwNumMaterials];
    pTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];

    for (DWORD i = 0; i < dwNumMaterials; i++)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, d3dxMaterials[i].pTextureFilename, -1, NULL, 0);
        std::wstring texFilename(len, 0);
        MultiByteToWideChar(CP_ACP, 0, d3dxMaterials[i].pTextureFilename, -1, &texFilename[0], len);

        pMaterials[i] = d3dxMaterials[i].MatD3D;
        pMaterials[i].Ambient = pMaterials[i].Diffuse;
        pTextures[i] = NULL;
        if (d3dxMaterials[i].pTextureFilename != NULL && len > 0)
        {
            if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
                                                 texFilename.c_str(),
                                                 &pTextures[i])))
            {
                MessageBox(NULL, _T("テクスチャの読み込みに失敗しました"), NULL, MB_OK);
            }
        }
    }
    pD3DXMtrlBuffer->Release();

    D3DXCreateEffectFromFile(
        g_pd3dDevice,
        _T("simple.fx"),
        NULL,
        NULL,
        D3DXSHADER_DEBUG,
        NULL,
        &pEffect,
        NULL
    );

    Sprite* sprCursor = new Sprite(g_pd3dDevice);
    sprCursor->Load(_T("cursor.png"));

    Sprite* sprBackground = new Sprite(g_pd3dDevice);
    sprBackground->Load(_T("background.png"));

    Sprite* sprPanelLeft = new Sprite(g_pd3dDevice);
    sprPanelLeft->Load(_T("panelLeft.png"));

    Sprite* sprPanelTop = new Sprite(g_pd3dDevice);
    sprPanelTop->Load(_T("craftPanel.png"));

    IFont* pFont = new Font(g_pd3dDevice);

    ISoundEffect* pSE = new SoundEffect();

    bool bEnglish = false;
    menu.Init(pFont, pSE, sprCursor, sprBackground, sprPanelLeft, sprPanelTop, bEnglish);

    if (!bEnglish)
    {
        std::vector<std::wstring> vsId;

        vsId.push_back(_T("item1"));
        vsId.push_back(_T("item2"));
        vsId.push_back(_T("item3"));
        vsId.push_back(_T("item4"));
        vsId.push_back(_T("item5"));
        vsId.push_back(_T("item6"));
        vsId.push_back(_T("item7"));
        vsId.push_back(_T("item8"));
        vsId.push_back(_T("item9"));
        vsId.push_back(_T("item10"));
        vsId.push_back(_T("item11"));
        vsId.push_back(_T("item12"));
        vsId.push_back(_T("item13"));
        vsId.push_back(_T("item14"));
        vsId.push_back(_T("item15"));
        vsId.push_back(_T("item16"));
        
        std::vector<std::wstring> vsName;

        vsName.push_back(_T("アイテムＡＡＡ"));
        vsName.push_back(_T("武器ＢＢＢ"));
        vsName.push_back(_T("アイテムＣ"));
        vsName.push_back(_T("アイテムＤ"));
        vsName.push_back(_T("アイテムＥ"));
        vsName.push_back(_T("アイテムＦ"));
        vsName.push_back(_T("アイテムＧ"));
        vsName.push_back(_T("アイテムＨ"));
        vsName.push_back(_T("アイテムＩ"));
        vsName.push_back(_T("アイテムＪ"));
        vsName.push_back(_T("アイテムＫ"));
        vsName.push_back(_T("アイテムＬ"));
        vsName.push_back(_T("アイテムＭ"));
        vsName.push_back(_T("アイテムＮ"));
        vsName.push_back(_T("アイテムＯ"));
        vsName.push_back(_T("アイテムＰ"));
        menu.SetOutputList(vsId, vsName);

        menu.SetCraftingItem(_T("アイテムＺＺＺＺＺ"), 24);

        vsId.clear();
        vsId.push_back(_T("アイテム１１１１１１１１１１１"));
        vsId.push_back(_T("アイテム２２２２２２２２２２２"));
        vsId.push_back(_T("アイテム３３３３３３３３３３３"));
        vsId.push_back(_T("アイテム４４４４４４４４４４４"));
        menu.SetCraftQue(vsId);

        std::wstring work;

        {
            work = _T("成果物の名前：アイテムＡＡＡ\n");
            work += _T("成果物の数：１\n");
            work += _T("成果物の強化度\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名あああ\n");
            work += _T("素材１の数：１０\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名いいい\n");
            work += _T("素材２の数：２０\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item1"), work);

            ISprite* sprite1 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item1"), _T("item1.png"), sprite1);
        }
        {
            work = _T("成果物の名前：武器ＢＢＢ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item2"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item2"), _T("item2.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＣ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item3"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item3"), _T("item3.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＤ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item4"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item4"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＥ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item5"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item5"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＦ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item6"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item6"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＧ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item7"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item7"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＨ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item8"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item8"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＩ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item9"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item9"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＪ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item10"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item10"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＫ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item11"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item11"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＬ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item12"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item12"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＭ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item13"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item13"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＮ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item14"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item14"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＯ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item15"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item15"), _T("item1.png"), sprite2);
        }
        {
            work = _T("成果物の名前：アイテムＰ\n");
            work += _T("成果物の数：２\n");
            work += _T("成果物の強化度：２\n");
            work += _T("\n");
            work += _T("素材１の名前：素材名かかかかか\n");
            work += _T("素材１の数：１１１\n");
            work += _T("素材１の強化度：１\n");
            work += _T("\n");
            work += _T("素材２の名前：素材名ききききき\n");
            work += _T("素材２の数：２２２\n");
            work += _T("素材２の強化度：２\n");

            menu.SetOutputInfo(_T("item16"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item16"), _T("item1.png"), sprite2);
        }
    }
    else
    {
        std::vector<std::wstring> vsId;

        vsId.push_back(_T("item_AAA"));
        vsId.push_back(_T("item_BBB"));
        vsId.push_back(_T("item_CCC"));
        vsId.push_back(_T("item_DDD"));
        vsId.push_back(_T("item_EEE"));
        vsId.push_back(_T("item_FFF"));
        vsId.push_back(_T("item_GGG"));
        vsId.push_back(_T("item_HHH"));
        vsId.push_back(_T("item_III"));
        vsId.push_back(_T("item_JJJ"));
        vsId.push_back(_T("item_KKK"));
        vsId.push_back(_T("item_LLL"));
        vsId.push_back(_T("item_MMM"));
        vsId.push_back(_T("item_NNN"));
        vsId.push_back(_T("item_OOO"));
        vsId.push_back(_T("item_PPP"));

        std::vector<std::wstring> vsName;

        vsName.push_back(_T("AAA"));
        vsName.push_back(_T("BBB"));
        vsName.push_back(_T("CCC"));
        vsName.push_back(_T("DDD"));
        vsName.push_back(_T("EEE"));
        vsName.push_back(_T("FFF"));
        vsName.push_back(_T("GGG"));
        vsName.push_back(_T("HHH"));
        vsName.push_back(_T("III"));
        vsName.push_back(_T("JJJ"));
        vsName.push_back(_T("KKK"));
        vsName.push_back(_T("LLL"));
        vsName.push_back(_T("MMM"));
        vsName.push_back(_T("NNN"));
        vsName.push_back(_T("OOO"));
        vsName.push_back(_T("PPP"));

        menu.SetOutputList(vsId, vsName);

        menu.SetCraftingItem(_T("item_zzzzzzzzzzzzzzzz"), 24);

        vsId.clear();
        vsId.push_back(_T("item_11111111111111"));
        vsId.push_back(_T("item_22222222222222"));
        vsId.push_back(_T("item_33333333333333"));
        vsId.push_back(_T("item_44444444444444"));
        menu.SetCraftQue(vsId);

        std::wstring work;

        {
            work = _T("Output name : item_AAA\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_AAA"), work);

            ISprite* sprite1 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_AAA"), _T("item1.png"), sprite1);
        }
        {
            work = _T("Output name : item_BBB\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_BBB"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_BBB"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_CCC\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_CCC"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_CCC"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_DDD\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_DDD"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_DDD"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_EEE\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_EEE"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_EEE"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_FFF\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_FFF"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_FFF"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_GGG\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_GGG"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_GGG"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_HHH\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_HHH"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_HHH"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_III\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_III"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_III"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_JJJ\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_JJJ"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_JJJ"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_KKK\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_KKK"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_KKK"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_LLL\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_LLL"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_LLL"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_MMM\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_MMM"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_MMM"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_NNN\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_NNN"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_NNN"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_OOO\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_OOO"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_OOO"), _T("item2.png"), sprite2);
        }
        {
            work = _T("Output name : item_PPP\n");
            work += _T("Output num : 1\n");
            work += _T("Output level\n");
            work += _T("\n");
            work += _T("Material 1 name : item_BBB\n");
            work += _T("Material 1 num : 10\n");
            work += _T("Material 1 level : 1\n");
            work += _T("\n");
            work += _T("Material 2 name : item_CCC\n");
            work += _T("Material 2 num : 20\n");
            work += _T("Material 2 level : 2\n");

            menu.SetOutputInfo(_T("item_PPP"), work);

            ISprite* sprite2 = new Sprite(g_pd3dDevice);
            menu.SetOutputImage(_T("item_PPP"), _T("item2.png"), sprite2);
        }
    }

    return S_OK;
}

VOID Cleanup()
{
    SAFE_RELEASE(pMesh);
    SAFE_RELEASE(g_pFont);
    SAFE_RELEASE(g_pd3dDevice);
    SAFE_RELEASE(g_pD3D);
}

VOID Render()
{
    if (NULL == g_pd3dDevice)
    {
        return;
    }
    f += 0.010f;

    D3DXMATRIX mat;
    D3DXMATRIX View, Proj;
    D3DXMatrixPerspectiveFovLH(&Proj, D3DXToRadian(45), 1600.0f / 900.0f, 1.0f, 1000.0f);
    D3DXVECTOR3 vec1(3 * sinf(f), 3, -3 * cosf(f));
    D3DXVECTOR3 vec2(0, 0, 0);
    D3DXVECTOR3 vec3(0, 1, 0);
    D3DXMatrixLookAtLH(&View, &vec1, &vec2, &vec3);
    D3DXMatrixIdentity(&mat);
    mat = mat * View * Proj;
    pEffect->SetMatrix("matWorldViewProj", &mat);

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);

    if (SUCCEEDED(g_pd3dDevice->BeginScene()))
    {
        wchar_t msg[128];
        wcscpy_s(msg, 128, _T("Cキーでクラフト画面を表示"));
        TextDraw(g_pFont, msg, 0, 0);

        pEffect->SetTechnique("BasicTec");
        UINT numPass;
        pEffect->Begin(&numPass, 0);
        pEffect->BeginPass(0);
        for (DWORD i = 0; i < dwNumMaterials; i++)
        {
            pEffect->SetTexture("texture1", pTextures[i]);
            pMesh->DrawSubset(i);
        }
        if (bShowMenu)
        {
            menu.Draw();
        }
        pEffect->EndPass();
        pEffect->End();
        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        Render();
        return 0;
    case WM_SIZE:
        InvalidateRect(hWnd, NULL, true);
        return 0;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case 'C':
            if (bShowMenu == false)
            {
                bShowMenu = true;
            }
            break;
        // メニューを表示している最中にメニューに表示されている内容を変える
        case VK_F2:
        {
            // menu.SetItem(itemInfoList);
        }
        case VK_UP:
            menu.Up();
            break;
        case VK_DOWN:
            menu.Down();
            break;
        case VK_LEFT:
            menu.Left();
            break;
        case VK_RIGHT:
            menu.Right();
            break;
        case VK_RETURN:
        {
            std::wstring result = menu.Into();
            if (result == _T("タイトル"))
            {
                bShowMenu = false;
            }
            else if (result == _T("最初から"))
            {
                bShowMenu = false;
            }
            break;
        }
        case VK_BACK:
        {
            std::wstring result;
            result = menu.Back();
            if (result == _T("EXIT"))
            {
                bShowMenu = false;
            }
            break;
        }
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_MOUSEMOVE:
    {
        POINTS mouse_p = MAKEPOINTS(lParam);
        menu.CursorOn(mouse_p.x, mouse_p.y);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        POINTS mouse_p = MAKEPOINTS(lParam);
        menu.Click(mouse_p.x, mouse_p.y);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        std::wstring result;
        result = menu.Back();
        if (result == _T("EXIT"))
        {
            bShowMenu = false;
        }
        break;
    }
    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (zDelta <= 0)
        {
            menu.Next();
        }
        else
        {
            menu.Previous();
        }
        break;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ INT)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      _T("Window1"), NULL };
    RegisterClassEx(&wc);

    RECT rect;
    SetRect(&rect, 0, 0, 1600, 900);
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    rect.right = rect.right - rect.left;
    rect.bottom = rect.bottom - rect.top;
    rect.top = 0;
    rect.left = 0;

    HWND hWnd = CreateWindow(_T("Window1"), _T("Hello DirectX9 World !!"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
        NULL, NULL, wc.hInstance, NULL);

    if (SUCCEEDED(InitD3D(hWnd)))
    {
        ShowWindow(hWnd, SW_SHOWDEFAULT);
        UpdateWindow(hWnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnregisterClass(_T("Window1"), wc.hInstance);
    return 0;
}
