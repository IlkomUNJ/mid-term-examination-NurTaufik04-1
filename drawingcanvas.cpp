#include "drawingcanvas.h"

void DrawingCanvas::dumpWindow(int x, int y, const CustomMatrix& mat) {
    std::cout << "\n--- Window at (" << x << ", " << y << ") ---" << std::endl;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            std::cout << (mat.isSet(r, c) ? "X " : "O ");
        }
        std::cout << std::endl;
    }
}

bool isIdealPattern(const CustomMatrix& mat) {
    if (mat.countSet() < 2) return false; 
    if (mat.isSet(1, 0) && mat.isSet(1, 1) && mat.isSet(1, 2)) return true;
    if (mat.isSet(0, 1) && mat.isSet(1, 1) && mat.isSet(2, 1)) return true;
    if (mat.isSet(0, 0) && mat.isSet(1, 1) && mat.isSet(2, 2)) return true;
    if (mat.isSet(0, 2) && mat.isSet(1, 1) && mat.isSet(2, 0)) return true;
    if (mat.countSet() == 2 && 
        ((mat.isSet(0,1) && mat.isSet(1,1)) || (mat.isSet(1,1) && mat.isSet(1,2)))) return true;
    return false;
}


DrawingCanvas::DrawingCanvas(QWidget *parent) : QWidget(parent) {
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    setStyleSheet("background-color: white; border: 1px solid gray;");
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    m_detectedSegments.clear();
    update();
}

void DrawingCanvas::paintLines(){
    isPaintLinesClicked = true;
    update();
}

void DrawingCanvas::segmentDetection(){
    QPixmap pixmap = this->grab();
    QImage image = pixmap.toImage();

    m_detectedSegments.clear(); 

    std::cout << "Starting segment detection and window dumping..." << std::endl;

    for(int i = 1; i < image.width()-1;i++){
        for(int j = 1; j < image.height()-1;j++){
            bool local_window[3][3] = {false};
            int set_pixels = 0;

            for(int m=-1;m<=1;m++){
                for(int n=-1;n<=1;n++){
                    QRgb rgbValue = image.pixel(i+m, j+n);
                    bool isSet = (rgbValue != 0xffffffff);
                    local_window[m+1][n+1] = isSet;
                    if (isSet) set_pixels++;
                }
            }

            CustomMatrix mat(local_window);
            if (set_pixels > 0) {
                dumpWindow(i, j, mat); 
            }
            
            if (isIdealPattern(mat)) {
                int x = i - 1; 
                int y = j - 1;
                m_detectedSegments.append(QRect(x, y, 3, 3));
            }
        }
    }
    update();
}

void DrawingCanvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }
    
    if(isPaintLinesClicked){
        std::cout << "paint lines block is called" << std::endl;
        pen.setColor(Qt::red);
        pen.setWidth(4);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        for(int i=0;i<m_points.size()-1;i+=2){
            painter.drawLine(m_points[i], m_points[i+1]);
        }
        isPaintLinesClicked = false;
    }
    
    QPen purplePen(QColor(128, 0, 128), 1);
    painter.setPen(purplePen);
    painter.setBrush(Qt::NoBrush); 

    for (const QRect& rect : m_detectedSegments) {
        painter.drawRect(rect);
    }
    pen.setColor(Qt::blue);
    painter.setPen(pen);
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    m_points.append(event->pos());
    update();
}
