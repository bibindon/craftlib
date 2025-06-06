#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace NSCraftLib
{
class ISprite
{
public:
    virtual void DrawImage(const int x, const int y, const int transparency = 255) = 0;
    virtual void Load(const std::wstring& filepath) = 0;
    virtual ~ISprite() {};
};

class IFont
{
public:
    virtual void DrawText_(const std::wstring& msg, const int x, const int y) = 0;
    virtual void Init(const bool bEnglish) = 0;
    virtual ~IFont() {};
};

class ISoundEffect
{
public:
    virtual void PlayMove() = 0;
    virtual void PlayClick() = 0;
    virtual void PlayBack() = 0;
    virtual void Init() = 0;
    virtual ~ISoundEffect() {};
};

class CraftLib
{
public:

    void Init(IFont* font,
              ISoundEffect* SE,
              ISprite* sprCursor,
              ISprite* sprBackground,
              ISprite* sprPanelLeft,
              ISprite* sprPanelTop,
              const bool bEnglish);

    void Finalize();

    void SetOutputList(const std::vector<std::wstring>& arg);
    void SetOutputInfo(const std::wstring& key, const std::wstring& value);
    void SetOutputImage(const std::wstring& key,
                        const std::wstring& imagePath,
                        ISprite* sprite);
    void SetCraftingItem(const std::wstring& name, const int progress);
    void SetCraftQue(const std::vector<std::wstring>& craftQue);

    std::wstring Up();
    std::wstring Down();
    std::wstring Right();
    std::wstring Left();
    std::wstring Into();
    std::wstring Back();
    std::wstring Next();
    std::wstring Previous();
    void CursorOn(const int x, const int y);
    std::wstring Click(const int x, const int y);
    void Draw();
    
private:

    enum class eFocus
    {
        OUTPUT,
        CONFIRM,
    };

    ISprite* m_sprCursor = NULL;
    ISprite* m_sprBackground = NULL;
    ISprite* m_sprPanelLeft = NULL;
    ISprite* m_sprPanelTop = NULL;
    IFont* m_font = NULL;
    ISoundEffect* m_SE = NULL;
    eFocus m_eFocus = eFocus::OUTPUT;

    std::vector<std::wstring> m_outputList;
    std::unordered_map<std::wstring, std::wstring> m_outputInfoMap;
    std::unordered_map<std::wstring, ISprite*> m_imageMap;

    const int LEFT_PANEL_PADDINGX = 50;
    const int LEFT_PANEL_PADDINGY = 13;

    const int LEFT_PANEL_WIDTH = 432;
    const int LEFT_PANEL_HEIGHT = 60;

    const int LEFT_PANEL_STARTX = 100;
    const int LEFT_PANEL_STARTY = 200;

    const int LEFT_PANEL_ROW_MAX = 10;

    // スクロール可能であることを考慮する
    // 上から画面上で何番目にカーソルがあるか。
    int m_leftCursor = 0;

    // カーソルが選択している要素がm_outputListの何番目の要素か。
    int m_leftSelect = 0;

    // 何番目のアイテムが一番上に表示されているか
    // スクロール可能なので一番上に表示されるアイテムはスクロールすると変わる。
    int m_leftBegin = 0;

    // はい・いいえ選択のときのカーソル状態
    int m_confirmCursor = 0;

    std::wstring m_craftingItem;
    int m_progress = 0;
    std::vector<std::wstring> m_craftQue;

    bool m_bEnglish = false;
};
}

