#ifndef ANIMATION_H
#define ANIMATION_H

#include<QHash>
#include<QReadWriteLock>

class AnimationCurve;

/**
 * @brief Describes an Animation and provides access
 *
 * An Animation consists of one or more AnimationCurves, which describe the rotation or translation of the skeleton´s segements
 * @sa AnimationCurve
 * @class Animation animation.h "core/agent/body/animation/animation.h"
 */
class Animation
{
public:
/**
 * @brief constructs a new Animation without any AnimationCurves
 *
 * @fn Animation
 */
    Animation();

    Animation(Animation *animation);
/**
 * @brief
 *
 * @sa AnimationCurve
 * @fn Animation
 * @param curves the AnimationCurves to add to this new Animation
 * @param name the name of this Animation
 */
    Animation(QHash<QString, AnimationCurve*> curves, QString name);

    /**
     * @brief calculate the length of this Animation
     *
     * The length is calculated as the max keyframe time of the longest AnimationCurve
     * @fn calculateLength
     */
    void calculateLength();

    void copyFromAnimation(Animation *animation);

    void copyFromAnimationCurves(QHash<QString, AnimationCurve*> curves);

    /**
     * @brief deletes the curve with name curve
     *
     * @param curveName the name of the curve to be deleted
     */
    void deleteCurve(const QString &curveName);

    /**
     * @brief deletes all curves
     *
     */
    void deleteCurves();

    /**
     * @brief returns the length of this Animation
     *
     * the length of an Animation is always the maximum time value of all keyframes of all AnimatoinCurve s
     * @fn getLength
     * @param calculateNew true if the length should re calculated, false if a cached length should be returned
     * @return qreal the length of this Animation
     */
    qreal getLength(bool calculateNew=false);


    virtual qreal getValue(const QString &curve, qreal time) const;

    /**
     * @brief returns, if this is a looped Animation
     *
     * @fn isLoopedAnimation
     * @return bool true, if this Animation is a loop animation
     */
    bool isLoopedAnimation() const { return m_isLoopedAnimation; }


    /**
     * @brief loads a brainiac animation file
     *
     * @param fileName
     * @return Animation
     */
    static Animation* loadAnimation(QString fileName);
    /**
     * @brief returns the filename
     *
     * the filename
     * @fn fileName
     * @return QString
     */
    QString fileName() const {return m_fileName; }

    /**
     * @brief adds a new keyframe into the AnimationCurve curve
     *
     * @fn addKeyFrame
     * @param curveName the name of the AnimationCurve
     * @param time
     * @param value
     */
    void addKeyFrame(QString &curveName, qreal time, qreal value);

    /**
     * @brief returns, if this Animation has AgentCurves
     *
     *
     * @fn hasAgentCurves
     * @return bool true, if this Animation has AgentCurves, false, if not
     */
    bool hasAgentCurves() const;
    /**
     * @brief the name of this Animation
     *
     * @fn name
     * @return QString the name
     */
    QString name() const { return m_name;}

    /**
     * @brief returns a modifiable reference to this Animation
     *
     * @fn curves
     * @return QHash<QString, AnimationCurve *> the reference to all the Animation´s AnimationCurves
     */
    QHash<QString, AnimationCurve*>& curves() {return m_curves;}
    QHash<QString, AnimationCurve*> curves() const {return m_curves;}

    /**
     * @brief returns a list of all AnimationCurve names
     *
     * @fn curveNames
     * @return QList<QString>
     */
    QList<QString> curveNames() const;

    bool saveAnimation();

    /**
     * @brief saves the animation with given absolute filename
     *
     * the filename also becomes the default filename
     * @sa saveAnimation()
     * @fn saveAnimation
     * @param fileName
     * @return bool
     */
    bool saveAnimation(QString &fileName);

    /**
     * @brief sets the filename of this Animation
     *
     * @fn setFileName
     * @param fileName
     */
    void setFileName(QString relativeFileName) {m_fileName=relativeFileName;}

    /**
     * @brief sets the name of this Animation
     *
     * @fn setName
     * @param name
     */
    virtual void setName(QString name) { m_name=name; }
    virtual ~Animation();
protected:
    qreal m_length;
    QHash<QString, AnimationCurve*> m_curves; /**< all AnimationCurves, QString is the curve name, that correspondents to the agent´s SkeletonNode channel @sa SkeletonNode @sa Channel */
    QString m_name; /**< name of this Animation */
    QString m_fileName; /**< filename relative to scenefile */
    bool m_isLoopedAnimation;
    mutable QReadWriteLock m_rwLock; /**< Lock to prevent reading from curves when the curves are changed */
};

#endif // ANIMATION_H
