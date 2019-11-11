/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HARDWARE_NEURALNETWORKS_V1_3_CALLBACKS_H
#define ANDROID_HARDWARE_NEURALNETWORKS_V1_3_CALLBACKS_H

#include <android-base/thread_annotations.h>
#include <android/hardware/neuralnetworks/1.0/IPreparedModelCallback.h>
#include <android/hardware/neuralnetworks/1.2/IPreparedModelCallback.h>
#include <android/hardware/neuralnetworks/1.3/IPreparedModelCallback.h>
#include <hidl/Status.h>
#include <condition_variable>
#include <mutex>

/*
 * The Callback classes are used internally by the NeuralNetworks runtime to
 * synchronize between different threads. An asynchronous task is launched
 * paired with a callback object. When a client thread requires the output being
 * generated by the asynchronous task, the client thread can wait for the result
 * and be blocked until it has completed. Any wait may safely be called
 * concurrently, even on the same callback object. When the asynchronous task
 * has finished its workload, it must immediately call "notify*". If the
 * asynchronous task has failed to launch, the function that tried to launch the
 * asynchronous task must immediately call "notify*". This "notify*" call
 * awakens any client threads waiting on the callback object.
 *
 * These classes exist to enable synchronization across HIDL. When
 * synchronization is only required in the same process, consider using
 * std::future, std::mutex, std::condition_variable, or std::experimental::latch
 * instead.
 */

namespace android::hardware::neuralnetworks::V1_3::implementation {

/**
 * The PreparedModelCallback class is used to receive the error status of
 * preparing a model as well as the prepared model from a task executing
 * asynchronously with respect to the runtime. If a calling thread calls wait
 * or get* on a PreparedModelCallback object and the corresponding asynchronous
 * task has not finished preparing the model, the calling thread will block
 * until the asynchronous task has called notify*.
 *
 * If the callback object is notified more than once, only the results of the
 * first call to notify* are used, and the results from subsequent calls are
 * discarded.
 *
 * This callback object is passed as an argument to IDevice::prepareModel*.
 */
class PreparedModelCallback : public IPreparedModelCallback {
  public:
    /**
     * IPreparedModelCallback::notify marks the callback object with the return
     * status of the asynchronous model preparation along with the prepared
     * model, and allows all prior and future wait calls on the
     * PreparedModelCallback object to proceed.
     *
     * One of IPreparedModelCallback::notify, IPreparedModelCallback::notify_1_2,
     * or IPreparedModelCallback::notify_1_3 must be called on a given
     * PreparedModelCallback object.
     *
     * If the callback object is notified more than once, only the results of
     * the first call to notify* are used, and the results from subsequent calls
     * are discarded.
     *
     * @param status Error status returned from asynchronously preparing the
     *     model; will be:
     *     - NONE if the asynchronous preparation was successful
     *     - DEVICE_UNAVAILABLE if driver is offline or busy
     *     - GENERAL_FAILURE if there is an unspecified error
     *     - INVALID_ARGUMENT if the input model is invalid
     * @param preparedModel Returned model that has been prepared for execution,
     *     nullptr if the model was unable to be prepared.
     */
    Return<void> notify(V1_0::ErrorStatus status,
                        const sp<V1_0::IPreparedModel>& preparedModel) override;

    /**
     * IPreparedModelCallback::notify_1_2 marks the callback object with the
     * return status of the asynchronous model preparation along with the
     * prepared model, and allows all prior and future wait calls on the
     * PreparedModelCallback object to proceed.
     *
     * One of IPreparedModelCallback::notify, IPreparedModelCallback::notify_1_2,
     * or IPreparedModelCallback::notify_1_3 must be called on a given
     * PreparedModelCallback object.
     *
     * If the callback object is notified more than once, only the results of
     * the first call to notify* are used, and the results from subsequent calls
     * are discarded.
     *
     * @param status Error status returned from asynchronously preparing the
     *     model; will be:
     *     - NONE if the asynchronous preparation was successful
     *     - DEVICE_UNAVAILABLE if driver is offline or busy
     *     - GENERAL_FAILURE if there is an unspecified error
     *     - INVALID_ARGUMENT if the input model is invalid
     * @param preparedModel Returned model that has been prepared for execution,
     *     nullptr if the model was unable to be prepared.
     */
    Return<void> notify_1_2(V1_0::ErrorStatus status,
                            const sp<V1_2::IPreparedModel>& preparedModel) override;

    /**
     * IPreparedModelCallback::notify_1_3 marks the callback object with the
     * return status of the asynchronous model preparation along with the
     * prepared model, and allows all prior and future wait calls on the
     * PreparedModelCallback object to proceed.
     *
     * One of IPreparedModelCallback::notify, IPreparedModelCallback::notify_1_2,
     * or IPreparedModelCallback::notify_1_3 must be called on a given
     * PreparedModelCallback object.
     *
     * If the callback object is notified more than once, only the results of
     * the first call to notify* are used, and the results from subsequent calls
     * are discarded.
     *
     * @param status Error status returned from asynchronously preparing the
     *     model; will be:
     *     - NONE if the asynchronous preparation was successful
     *     - DEVICE_UNAVAILABLE if driver is offline or busy
     *     - GENERAL_FAILURE if there is an unspecified error
     *     - INVALID_ARGUMENT if the input model is invalid
     * @param preparedModel Returned model that has been prepared for execution,
     *     nullptr if the model was unable to be prepared.
     */
    Return<void> notify_1_3(V1_0::ErrorStatus status,
                            const sp<V1_2::IPreparedModel>& preparedModel) override;

    /**
     * PreparedModelCallback::wait blocks until notify* has been called on the
     * callback object.
     */
    void wait() const;

    /**
     * Retrieves the error status returned from the asynchronous task launched
     * by IDevice::prepareModel*. If IDevice::prepareModel* has not finished
     * asynchronously preparing the model, this call will block until the
     * asynchronous task notifies the object.
     *
     * @return status Error status returned from asynchronously preparing the
     *     model; will be:
     *     - NONE if the asynchronous preparation was successful
     *     - DEVICE_UNAVAILABLE if driver is offline or busy
     *     - GENERAL_FAILURE if there is an unspecified error
     *     - INVALID_ARGUMENT if the input model is invalid
     */
    V1_0::ErrorStatus getStatus() const;

    /**
     * Retrieves the model that has been prepared for execution from the
     * asynchronous task launched by IDevice::prepareModel*. If
     * IDevice::prepareModel* has not finished asynchronously preparing the
     * model, this call will block until the asynchronous task notifies the
     * object.
     *
     * @return preparedModel Returned model that has been prepared for
     *     execution, nullptr if the model was unable to be prepared.
     */
    sp<V1_0::IPreparedModel> getPreparedModel() const;

  private:
    mutable std::mutex mMutex;
    mutable std::condition_variable mCondition;
    bool mNotified GUARDED_BY(mMutex) = false;
    V1_0::ErrorStatus mErrorStatus = V1_0::ErrorStatus::GENERAL_FAILURE;
    sp<V1_0::IPreparedModel> mPreparedModel;
};

}  // namespace android::hardware::neuralnetworks::V1_3::implementation

#endif  // ANDROID_HARDWARE_NEURALNETWORKS_V1_3_CALLBACKS_H
