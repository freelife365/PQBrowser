#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#pragma once

namespace DuiLib {
enum
{
    EVENT_TIME_ID = 100,
};

class UILIB_API CGifAnimUI : public CControlUI
{
    DECLARE_DUICONTROL(CGifAnimUI)
public:
    CGifAnimUI();
    ~CGifAnimUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    void DoInit() override;
    bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) override;
    void DoEvent(TEventUI& event) override;
    void SetVisible(bool bVisible = true) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    void SetBkImage(LPCTSTR pStrImage);
    LPCTSTR GetBkImage();

    void SetAutoPlay(bool bIsAuto = true);
    bool IsAutoPlay() const;
    void SetAutoSize(bool bIsAuto = true);
    bool IsAutoSize() const;
    void PlayGif();
    void PauseGif();
    void StopGif();

    void OnTimer(UINT_PTR idEvent);

private:
    void InitGifImage();
    void DeleteGif();
    void DrawFrame(HDC hDC); // 绘制GIF每帧

private:
    Gdiplus::Image* m_pGifImage = nullptr;

    // 帧与帧之间间隔时间
    Gdiplus::PropertyItem* m_pPropertyItem = nullptr; 

    UINT m_nFrameCount = 0;    // gif图片总帧数
    UINT m_nFramePosition = 0; // 当前放到第几帧
    bool m_bIsAutoPlay = true;  // 是否自动播放gif
    bool m_bIsAutoSize = false; // 是否自动根据图片设置大小
    bool m_bIsPlaying = false;
};
} // namespace DuiLib

#endif // GifAnimUI_h__
