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
                    ISprite* sprPanelLeft)
{
    m_font = font;
    m_SE = SE;
    m_sprCursor = sprCursor;
    m_sprBackground = sprBackground;
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

std::string CraftLib::Up()
{
    if (m_eFocus == eFocus::OUTPUT)
    {
        if (m_leftSelect >= 1)
        {
            m_leftSelect--;
            m_SE->PlayMove();
        }
        // カーソルが一番上にあるときに上ボタンを押されたら
        // カーソルはそのままでリストが下に移動する
        if (m_leftCursor != 0)
        {
            m_leftCursor--;
        }
        else
        {
            if (m_leftSelect != 0)
            {
                m_leftBegin--;
            }
        }
    }
    return m_outputList.at(m_leftSelect);
}

std::string CraftLib::Down()
{
    if (m_eFocus == eFocus::OUTPUT)
    {
        if (m_leftSelect <= (int)m_outputList.size() - 2)
        {
            m_leftSelect++;
            m_SE->PlayMove();
        }

        // カーソルが一番下にあるときに下ボタンを押されたら
        // カーソルはそのままでリストが上に移動する
        if (m_leftCursor != LEFT_PANEL_ROW_MAX - 1)
        {
            m_leftCursor++;
        }
        else
        {
            if (m_leftSelect != (int)m_outputList.size() - 1)
            {
                m_leftBegin++;
            }
        }
    }
    return m_outputList.at(m_leftSelect);
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
    if ((int)m_outputList.size() >= LEFT_PANEL_ROW_MAX)
    {
        for (int i = 0; i < LEFT_PANEL_ROW_MAX; ++i) {
            m_sprPanelLeft->DrawImage(LEFT_PANEL_STARTX, LEFT_PANEL_STARTY + (LEFT_PANEL_HEIGHT * i));
        }
    }
    else
    {
        for (std::size_t i = 0; i < m_outputList.size(); ++i) {
            m_sprPanelLeft->DrawImage(LEFT_PANEL_STARTX, LEFT_PANEL_STARTY + (LEFT_PANEL_HEIGHT * i));
        }
    }

    // 左の列の文字、成果物の名称
    if ((int)m_outputList.size() >= LEFT_PANEL_ROW_MAX)
    {
        for (int i = m_leftBegin; i < m_leftBegin + LEFT_PANEL_ROW_MAX; ++i)
        {
            m_font->DrawText_(m_outputList.at(i),
                              LEFT_PANEL_STARTX + LEFT_PANEL_PADDINGX,
                              LEFT_PANEL_STARTY + LEFT_PANEL_PADDINGY + ((i - m_leftBegin) * LEFT_PANEL_HEIGHT));
        }
    }
    else
    {
        for (std::size_t i = 0; i < m_outputList.size(); ++i)
        {
            m_font->DrawText_(m_outputList.at(i),
                              LEFT_PANEL_STARTX + LEFT_PANEL_PADDINGX,
                              LEFT_PANEL_STARTY + LEFT_PANEL_PADDINGY + (i * LEFT_PANEL_HEIGHT));
        }
    }

    // 右側の説明テキスト

    std::string work;
    work = m_outputList.at(m_leftSelect);

    if (m_imageMap.find(work) != m_imageMap.end())
    {
        m_imageMap.at(work)->DrawImage(550, 300);
    }

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

    m_sprCursor->DrawImage(80, 205 + (m_leftCursor * 60));

}

