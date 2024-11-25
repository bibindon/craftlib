#include "CraftLib.h"
#include <sstream>

using namespace NSCraftLib;

static std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}

void CraftLib::Init(IFont* font,
                    ISoundEffect* SE,
                    ISprite* sprCursor,
                    ISprite* sprBackground,
                    ISprite* sprPanel,
                    ISprite* sprPanelLeft)
{
    m_font = font;
    m_SE = SE;
    m_sprCursor = sprCursor;
    m_sprBackground = sprBackground;
    m_sprPanel = sprPanel;
    m_sprPanelLeft = sprPanelLeft;
}

void NSCraftLib::CraftLib::SetOutputList(const std::vector<std::string>& arg)
{
    m_outputList = arg;
}

void NSCraftLib::CraftLib::SetOutputInfo(const std::string& key, const std::string& value)
{
    m_outputInfoMap[key] = value;
}

void NSCraftLib::CraftLib::SetOutputImage(const std::string& key,
                                          const std::string& imagePath,
                                          ISprite* sprite)
{
    sprite->Load(imagePath);
    m_imageMap[key] = sprite;

}

// TODO スクロールできることに注意
std::string CraftLib::Up()
{
    if (m_eFocus == eFocus::OUTPUT)
    {
        if (m_outputCursorIndex >= 1)
        {
            m_outputCursorIndex--;
            m_SE->PlayMove();
        }
    }
    return m_outputList.at(m_outputCursorIndex);
}

std::string CraftLib::Down()
{
    if (m_eFocus == eFocus::OUTPUT)
    {
        if (m_outputCursorIndex <= (int)m_outputList.size() - 2)
        {
            m_outputCursorIndex++;
            m_SE->PlayMove();
        }
    }
    return m_outputList.at(m_outputCursorIndex);
}

std::string CraftLib::Right()
{
    return std::string();
}

std::string CraftLib::Left()
{
    return std::string();
}

std::string CraftLib::Into()
{
    return std::string();
}

std::string CraftLib::Back()
{
    return std::string();
}

std::string NSCraftLib::CraftLib::Next()
{
    return std::string();
}

std::string NSCraftLib::CraftLib::Previous()
{
    return std::string();
}

void CraftLib::Click(const int x, const int y)
{
    m_SE->PlayClick();
}

void CraftLib::RightClick(const int x, const int y)
{
    m_SE->PlayClick();
}

void CraftLib::Draw()
{
    // 背景
    m_sprBackground->DrawImage(0, 0);

    // 左の列の背景
    for (int i = 0; i < LEFT_PANEL_ROW_MAX; ++i)
    {
        m_sprPanelLeft->DrawImage(LEFT_PANEL_STARTX, LEFT_PANEL_STARTY + (LEFT_PANEL_HEIGHT * i));
    }

    // 左の列の文字、成果物の名称
    for (std::size_t i = 0; i < m_outputList.size(); ++i)
    {
        m_font->DrawText_(m_outputList.at(i),
                          LEFT_PANEL_STARTX + LEFT_PANEL_PADDINGX,
                          LEFT_PANEL_STARTY + LEFT_PANEL_PADDINGY + (i * LEFT_PANEL_HEIGHT));
    }

    // 右側の説明テキスト

    std::string work;
    work = m_outputList.at(m_outputCursorIndex);

    m_imageMap.at(work)->DrawImage(550, 300);

    std::string detail = m_outputInfoMap.at(work);
    std::vector<std::string> details = split(detail, '\n');

    for (std::size_t i = 0; i < details.size(); ++i)
    {
        m_font->DrawText_(
            details.at(i),
            1100,
            250 + i * 40
        );
    }

    m_sprCursor->DrawImage(80, 205 + (m_outputCursorIndex * 60));

}

