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
    virtual void Load(const std::string& filepath) = 0;
    virtual ~ISprite() {};
};

class IFont
{
public:
    virtual void DrawText_(const std::string& msg, const int x, const int y) = 0;
    virtual void Init() = 0;
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
              ISprite* sprPanel,
              ISprite* sprPanelLeft);

    void SetOutputList(const std::vector<std::string>& arg);
    void SetOutputInfo(const std::string& key, const std::string& value);
    void SetOutputImage(const std::string& key,
                        const std::string& imagePath,
                        ISprite* sprite);

    std::string Up();
    std::string Down();
    std::string Right();
    std::string Left();
    std::string Into();
    std::string Back();
    std::string Next();
    std::string Previous();
    void Click(const int x, const int y);
    void RightClick(const int x, const int y);
    void Draw();
    
private:

    enum class eFocus
    {
        OUTPUT,
        CONFIRM,
    };

    ISprite* m_sprCursor;
    ISprite* m_sprBackground;
    ISprite* m_sprPanel; // TODO Remove
    ISprite* m_sprPanelLeft;
    IFont* m_font;
    ISoundEffect* m_SE;
    eFocus m_eFocus = eFocus::OUTPUT;

    std::vector<std::string> m_outputList;
    std::unordered_map<std::string, std::string> m_outputInfoMap;
//    std::unordered_map<std::string, std::string> m_imagePathMap;
    std::unordered_map<std::string, ISprite*> m_imageMap;

    const int LEFT_PANEL_PADDINGX = 50;
    const int LEFT_PANEL_PADDINGY = 13;

    const int LEFT_PANEL_WIDTH = 432;
    const int LEFT_PANEL_HEIGHT = 60;

    const int LEFT_PANEL_STARTX = 100;
    const int LEFT_PANEL_STARTY = 200;

    const int LEFT_PANEL_ROW_MAX = 10;



    int m_outputCursorIndex = 0;
};
}

