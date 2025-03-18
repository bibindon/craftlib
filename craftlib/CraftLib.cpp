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
                    ISprite* sprPanelLeft,
                    ISprite* sprPanelTop)
{
    m_font = font;
    m_SE = SE;
    m_sprCursor = sprCursor;
    m_sprBackground = sprBackground;
    m_sprPanelLeft = sprPanelLeft;
    m_sprPanelTop = sprPanelTop;
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

void NSCraftLib::CraftLib::SetCraftingItem(const std::string& name, const int progress)
{
    m_craftingItem = name;
    m_progress = progress;
}

void NSCraftLib::CraftLib::SetCraftQue(const std::vector<std::string>& craftQue)
{
    m_craftQue = craftQue;
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
        else if (m_leftCursor == 0)
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
        else if (m_leftCursor == LEFT_PANEL_ROW_MAX - 1)
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
    if (m_eFocus == eFocus::CONFIRM)
    {
        if (m_confirmCursor == 0)
        {
            m_confirmCursor = 1;
            m_SE->PlayMove();
        }
    }
    return std::string();
}

std::string CraftLib::Left()
{
    if (m_eFocus == eFocus::CONFIRM)
    {
        if (m_confirmCursor == 1)
        {
            m_confirmCursor = 0;
            m_SE->PlayMove();
        }
    }
    return std::string();
}

std::string CraftLib::Into()
{
    std::string result;
    if (m_eFocus == eFocus::OUTPUT)
    {
        m_eFocus = eFocus::CONFIRM;
        m_SE->PlayClick();
    }
    else if (m_eFocus == eFocus::CONFIRM)
    {
        if (m_confirmCursor == 0)
        {
            m_SE->PlayClick();
            result = m_outputList.at(m_leftSelect);
        }
        else if (m_confirmCursor == 1)
        {
            m_SE->PlayBack();
            m_eFocus = eFocus::OUTPUT;
        }
    }
    return result;
}

std::string CraftLib::Back()
{
    std::string result;
    if (m_eFocus == eFocus::OUTPUT)
    {
        result = "EXIT";
        m_SE->PlayBack();
    }
    else if (m_eFocus == eFocus::CONFIRM)
    {
        m_eFocus = eFocus::OUTPUT;
        m_SE->PlayBack();
    }
    return result;
}

std::string NSCraftLib::CraftLib::Next()
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
        else if (m_leftCursor == LEFT_PANEL_ROW_MAX - 1)
        {
            if (m_leftSelect != (int)m_outputList.size() - 1)
            {
                m_leftBegin++;
            }
        }
    }
    return m_outputList.at(m_leftSelect);
}

std::string NSCraftLib::CraftLib::Previous()
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
        else if (m_leftCursor == 0)
        {
            if (m_leftSelect != 0)
            {
                m_leftBegin--;
            }
        }
    }
    return m_outputList.at(m_leftSelect);
}

void CraftLib::CursorOn(const int x, const int y)
{
    if (m_eFocus == eFocus::OUTPUT)
    {
        int previousCursor = m_leftCursor;
        if (LEFT_PANEL_STARTX < x && x <= LEFT_PANEL_STARTX + LEFT_PANEL_WIDTH)
        {
            if (LEFT_PANEL_STARTY < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 0;
                m_leftSelect = 0 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 1;
                m_leftSelect = 1 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 2;
                m_leftSelect = 2 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 3;
                m_leftSelect = 3 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 4;
                m_leftSelect = 4 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 5;
                m_leftSelect = 5 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 6;
                m_leftSelect = 6 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 7;
                m_leftSelect = 7 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 8;
                m_leftSelect = 8 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 10)
            {
                m_eFocus = eFocus::OUTPUT;
                m_leftCursor = 9;
                m_leftSelect = 9 + m_leftBegin;
            }
        }
        if (previousCursor != m_leftCursor)
        {
            m_SE->PlayMove();
        }
    }
    else if (m_eFocus == eFocus::CONFIRM)
    {
        int previousCursor = m_confirmCursor;
        if (LEFT_PANEL_STARTY < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1)
        {
            if (m_leftSelect == 0)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2)
        {
            if (m_leftSelect == 1)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3)
        {
            if (m_leftSelect == 2)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4)
        {
            if (m_leftSelect == 3)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5)
        {
            if (m_leftSelect == 4)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6)
        {
            if (m_leftSelect == 5)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7)
        {
            if (m_leftSelect == 6)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8)
        {
            if (m_leftSelect == 7)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9)
        {
            if (m_leftSelect == 8)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 10)
        {
            if (m_leftSelect == 9)
            {
                if (550 < x && x <= 850)
                {
                    m_confirmCursor = 0;
                }
                else if (850 < x && x <= 550 + LEFT_PANEL_WIDTH)
                {
                    m_confirmCursor = 1;
                }
            }
        }

        if (previousCursor != m_confirmCursor)
        {
            m_SE->PlayMove();
        }
    }
}

std::string CraftLib::Click(const int x, const int y)
{
    std::string result;
    m_SE->PlayClick();
    if (m_eFocus == eFocus::OUTPUT)
    {
        if (LEFT_PANEL_STARTX < x && x <= LEFT_PANEL_STARTX + LEFT_PANEL_WIDTH)
        {
            if (LEFT_PANEL_STARTY < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 0;
                m_leftSelect = 0 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 1;
                m_leftSelect = 1 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 2;
                m_leftSelect = 2 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 3;
                m_leftSelect = 3 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 4;
                m_leftSelect = 4 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 5;
                m_leftSelect = 5 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 6;
                m_leftSelect = 6 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 7;
                m_leftSelect = 7 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 8;
                m_leftSelect = 8 + m_leftBegin;
            }
            else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 10)
            {
                m_eFocus = eFocus::CONFIRM;
                m_leftCursor = 9;
                m_leftSelect = 9 + m_leftBegin;
            }
        }
    }
    else if (m_eFocus == eFocus::CONFIRM)
    {
        if (LEFT_PANEL_STARTY < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1)
        {
            if (m_leftSelect == 0)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 1 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2)
        {
            if (m_leftSelect == 1)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 2 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3)
        {
            if (m_leftSelect == 2)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 3 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4)
        {
            if (m_leftSelect == 3)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 4 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5)
        {
            if (m_leftSelect == 4)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 5 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6)
        {
            if (m_leftSelect == 5)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 6 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7)
        {
            if (m_leftSelect == 6)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 7 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8)
        {
            if (m_leftSelect == 7)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 8 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9)
        {
            if (m_leftSelect == 8)
            {
                result = Into();
            }
        }
        else if (LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 9 < y && y <= LEFT_PANEL_STARTY + LEFT_PANEL_HEIGHT * 10)
        {
            if (m_leftSelect == 9)
            {
                result = Into();
            }
        }
    }
    return result;
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

    // 中央の画像
    std::string work;
    work = m_outputList.at(m_leftSelect);

    if (m_imageMap.find(work) != m_imageMap.end())
    {
        m_imageMap.at(work)->DrawImage(550, 300);
    }

    // 右側の説明テキスト
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

    // はい・いいえの表示
    if (m_eFocus == eFocus::CONFIRM)
    {
        m_sprPanelLeft->DrawImage(550, 200 + (m_leftCursor * LEFT_PANEL_HEIGHT));
        m_font->DrawText_(
            "クラフトする　　しない",
            650,
            200 + LEFT_PANEL_PADDINGY + (m_leftCursor * LEFT_PANEL_HEIGHT)
        );
    }

    // クラフト中のアイテムと進捗度の表示
    {
        m_sprPanelTop->DrawImage(90, 73, 123);
        m_sprPanelTop->DrawImage(370, 73);
        m_font->DrawText_("クラフト中のアイテム", 100, 85);

        // 8文字まで表示（全角8文字）
        if (m_craftingItem.size() <= 8*2)
        {
            m_font->DrawText_(m_craftingItem, 400, 85);
        }
        // 表示しきれないときは7文字目まで表示＋「…」
        else
        {
            m_font->DrawText_(m_craftingItem.substr(0, 14) + "…", 400, 85);
        }

        m_font->DrawText_("進捗度： " + std::to_string(m_progress) + " ％", 700, 85);
    }

    // 予約リストの表示
    {
        m_sprPanelTop->DrawImage(90, 133, 123);
        m_font->DrawText_("予約リスト", 155, 145);

        m_sprPanelTop->DrawImage(370, 133);
        m_sprPanelTop->DrawImage(650, 133);
        m_sprPanelTop->DrawImage(930, 133);
        m_sprPanelTop->DrawImage(1210, 133);

        for (std::size_t i = 0; i < m_craftQue.size(); ++i)
        {
            if (i >= 4)
            {
                break;
            }
            std::string work = m_craftQue.at(i);

            // 8文字まで表示（全角8文字）
            if (work.size() <= 8*2)
            {
                m_font->DrawText_(work, 400 + (280 * i), 145);
            }
            // 表示しきれないときは7文字目まで表示＋「…」
            else
            {
                m_font->DrawText_(work.substr(0, 14) + "…", 400 + (280 * i), 145);
            }
        }
    }


    // カーソルの表示
    if (m_eFocus == eFocus::OUTPUT)
    {
        m_sprCursor->DrawImage(80, 205 + (m_leftCursor * 60));
    }
    else if (m_eFocus == eFocus::CONFIRM)
    {
        m_sprCursor->DrawImage(570 + (m_confirmCursor * 160), 205 + (m_leftCursor * 60));
    }


}

